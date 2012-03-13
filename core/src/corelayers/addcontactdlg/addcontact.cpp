/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "addcontact.h"
#include "ui_addcontact.h"
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <qutim/servicemanager.h>
#include <QStringBuilder>
#include <QToolButton>
#include <QPushButton>
#include <qutim/systemintegration.h>

namespace Core
{

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
}

AddContactModule::~AddContactModule()
{

}

void AddContactModule::show(Account *account, const QString &id,
                            const QString &name, const QStringList &tags)
{
	AddContact *addContact = new AddContact(account);
	addContact->setContactId(id);
	addContact->setContactName(name);
	addContact->setContactTags(tags);
	centerizeWidget(addContact);
	SystemIntegration::show(addContact);
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

void AddContact::setContactId(const QString &id)
{
	d_func()->ui->editId->setText(id);
}

void AddContact::setContactName(const QString &name)
{
	d_func()->ui->editName->setText(name);
}

void AddContact::setContactTags(const QStringList &tags)
{
	Q_UNUSED(tags);
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
	ChatSession *session = ChatLayer::instance()->getSession(d->account, d->ui->editId->text(), true);
	if (session)
		session->activate();
}

void AddContact::onShowInfoClicked()
{
	Q_D(AddContact);
	QObject *obj = ServiceManager::getByName("ContactInfo");
	ChatUnit *unit = d->account->getUnit(d->ui->editId->text(), true);
	if (unit)
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

