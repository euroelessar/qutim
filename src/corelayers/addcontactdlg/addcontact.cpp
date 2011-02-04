#include "addcontact.h"
#include "ui_addcontact.h"
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/messagesession.h>
#include <qutim/servicemanager.h>
#include <QStringBuilder>
#include <QToolButton>
#include <QPushButton>
#include <qutim/systemintegration.h>

namespace Core
{

void checkContact(QAction *action, Contact *contact)
{
	action->setEnabled(contact->account()->status() != Status::Offline);
	bool isInList = contact->isInList();
	action->setText(isInList ? QT_TRANSLATE_NOOP("AddContact", "Remove from roster") :
							   QT_TRANSLATE_NOOP("AddContact", "Add to roster"));
	action->setIcon(isInList ? Icon("list-remove") :
							   Icon("list-add"));
}

bool isSupportAddContact()
{
	foreach (Protocol *p,Protocol::all()) {
		bool support = p->data(qutim_sdk_0_3::Protocol::ProtocolContainsContacts).toBool();
		if (support) {
			foreach (Account *a,p->accounts()) {
				if (a->status() != Status::Offline) {
					return true;
				}
			}
		}
	}
	return false;
}

class AddContactPrivate
{
public:
	QHash<QString, Account *> accounts;
	QHash<QString, QToolButton *> buttons;
	Account *account;
	Ui::AddContact *ui;
};

AddContactModule::AddContactModule()
{
	QObject *contactList = ServiceManager::getByName("ContactList");
	if (contactList) {
		m_addUserGen.reset(new ActionGenerator(Icon("list-add-user"),
											   QT_TRANSLATE_NOOP("AddContact", "Add contact"),
											   this, SLOT(show())));
		MenuController *controller = qobject_cast<MenuController*>(contactList);
		Q_ASSERT(controller);
		controller->addAction(m_addUserGen.data());
	}

	m_addRemoveGen.reset(new ActionGenerator(QIcon(),
											 QT_TRANSLATE_NOOP("AddContact", "Unavailable"),
											 this, SLOT(onContactAddRemoveAction(QObject*))));
	m_addRemoveGen->addHandler(ActionCreatedHandler, this);
	MenuController::addAction<Contact>(m_addRemoveGen.data());
}

AddContactModule::~AddContactModule()
{

}

bool AddContactModule::event(QEvent *ev)
{
	if (ev->type() == ActionCreatedEvent::eventType()) {
		ActionCreatedEvent *event = static_cast<ActionCreatedEvent*>(ev);
		Contact *contact = qobject_cast<Contact*>(event->controller());
		Q_ASSERT(contact);
		checkContact(event->action(),contact);
		connect(contact->account(), SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
				this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
		ev->accept();
	}
	return QObject::event(ev);
}

void AddContactModule::onAccountStatusChanged(Status)
{
	QMap<const QObject*, QAction*> actions = m_addRemoveGen->actions();
	QMap<const QObject*, QAction*>::const_iterator it = actions.constBegin();
	for(;it != actions.constEnd(); it++) {
		//TODO may be possible use reinterpret_cast?
		const Contact *contact = qobject_cast<const Contact*>(it.key());
		Q_ASSERT(contact);
		checkContact(it.value(), const_cast<Contact*>(contact));
	}
}

void AddContactModule::onContactAddRemoveAction(QObject *obj)
{
	Contact *contact = qobject_cast<Contact*>(obj);
	Q_ASSERT(contact);
	contact->setInList(!contact->isInList());
}

void AddContactModule::show()
{
	AddContact *addContact = new AddContact();
	centerizeWidget(addContact);
	SystemIntegration::show(addContact);
}

AddContact::AddContact(Account *account, QWidget *parent) : QDialog(parent), d_ptr(new AddContactPrivate())
{
	Q_D(AddContact);
	d->ui = new Ui::AddContact();
	d->ui->setupUi(this);
	if (account) {
		setAccount(account);
	} else {
		d->ui->stackedWidget->setCurrentIndex(0);
		foreach (Protocol *protocol, Protocol::all())
			if (protocol->data(Protocol::ProtocolContainsContacts).toBool() && !protocol->accounts().isEmpty())
				foreach (Account *acc, protocol->accounts()) {
					QToolButton *button = new QToolButton(d->ui->accountPage);
					button->setText(acc->id());
					QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
					button->setSizePolicy(sizePolicy);
#if !defined(Q_OS_SYMBIAN)
					button->setAutoRaise(true);
#endif
					connect(button, SIGNAL(clicked()), SLOT(setAccount()));
					d->ui->accountLayout->insertWidget(d->ui->accountLayout->count()-1, button);
					d->accounts.insert(acc->id(), acc);
					d->buttons.insert(acc->id(), button);
					changeState(acc, acc->status());
					connect(acc, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
							SLOT(changeState(qutim_sdk_0_3::Status)));
				}
		if (d->accounts.count() == 1)
			setAccount(d->accounts.values().at(0));
	}

	d->ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
	connect(d->ui->buttonBox,SIGNAL(accepted()),SLOT(on_okButton_clicked()));
	connect(d->ui->buttonBox,SIGNAL(rejected()),SLOT(on_cancelButton_clicked()));
	QPushButton *button = d->ui->buttonBox->addButton(tr("Start chat"),
													  QDialogButtonBox::ActionRole);
	connect(button, SIGNAL(clicked()), this, SLOT(onStartChatClicked()));
	if (ServiceManager::getByName("ContactInfo")) {
		button = d->ui->buttonBox->addButton(tr("Show info"), QDialogButtonBox::ActionRole);
		connect(button, SIGNAL(clicked()), this, SLOT(onShowInfoClicked()));
	}
	connect(d->ui->stackedWidget,SIGNAL(currentChanged(int)),SLOT(currentChanged(int)));
}

AddContact::~AddContact()
{
}

void AddContact::setAccount()
{
	if (QToolButton *button = qobject_cast<QToolButton *>(sender()))
		setAccount(d_func()->accounts.value(button->text()));
}

void AddContact::setAccount(Account *account)
{
	Q_D(AddContact);
	d->account = account;
	d->ui->stackedWidget->setCurrentIndex(1);
	d->ui->IDLabel->setText(account->protocol()->data(Protocol::ProtocolIdName).toString() % QLatin1Literal(":"));
}

void AddContact::on_okButton_clicked()
{
	Q_D(AddContact);
	Contact *contact = qobject_cast<Contact *>(d->account->getUnit(d->ui->editId->text(), true));
	if (contact) {
		contact->setInList(true);
		contact->setName(d->ui->editName->text());
		deleteLater();
	}
}

void AddContact::on_cancelButton_clicked()
{
	deleteLater();
}

void AddContact::onStartChatClicked()
{
	Q_D(AddContact);
	ChatLayer::instance()->getSession(d->account, d->ui->editId->text(), true)->activate();
}

void AddContact::onShowInfoClicked()
{
	Q_D(AddContact);
	QObject *obj = ServiceManager::getByName("ContactInfo");
	ChatUnit *unit = d->account->getUnit(d->ui->editId->text(), true);
	QMetaObject::invokeMethod(obj, "show", Q_ARG(QObject*, unit));
}

void AddContact::changeState(const qutim_sdk_0_3::Status &status)
{
	if (Account *account = qobject_cast<Account *>(sender()))
		changeState(account, status);
}

void AddContact::changeState(Account *account, const qutim_sdk_0_3::Status &status)
{
	if (QToolButton *button = d_func()->buttons.value(account->id())) {
		if (status == Status::Connecting || status == Status::Offline) {
			button->setEnabled(false);
			button->setToolTip(tr("Account must be online"));
		} else {
			button->setEnabled(true);
			button->setToolTip("");
		}
	}
}

void AddContact::currentChanged(int index)
{
	if (index == 1) {
		d_func()->ui->buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	}
}

}
