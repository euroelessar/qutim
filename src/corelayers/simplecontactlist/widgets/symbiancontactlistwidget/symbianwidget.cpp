/****************************************************************************
 *  symbianwidget.cpp
 *
 *  (c) LLC INRUSCOM, Russia
 *
*****************************************************************************/

#include "symbianwidget.h"
#include <qutim/utils.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/utils.h>
#include <qutim/actiontoolbar.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/systemintegration.h>
#include <simplestatusdialog.h>
#include <qutim/servicemanager.h>
#include <QAbstractItemDelegate>
#include <qutim/protocol.h>
#include <qutim/shortcut.h>
#include <QLineEdit>
#include <qutim/metacontact.h>
#include <qutim/actiongenerator.h>
#include <QTimer>
#include <QVBoxLayout>
#include <abstractcontactmodel.h>
#include <simplecontactlist.h>

namespace Core {
namespace SimpleContactList {

//TODO move to core module and rewrite
static bool isStatusChange(const qutim_sdk_0_3::Status &status)
{
	if (status.type() == Status::Offline) {
		foreach(Protocol *proto, Protocol::all()) {
			foreach(Account *a, proto->accounts()) {
				debug() << a->status().name() << a->status().type();
				if (a->status().type()!=Status::Offline)
					return false;
			}
		}
		return true;
	}
	else
		return true;
}

SymbianWidget::SymbianWidget()
{
	setWindowIcon(Icon("qutim"));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	m_model = ServiceManager::getByName<AbstractContactModel*>("ContactModel");
	m_view = new TreeView(m_model, this);
	layout->addWidget(m_view);
	m_view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate*>("ContactDelegate"));

	QMenu *statusMenu = new QMenu(this);

	m_searchBar = new QLineEdit(this);
	m_searchBar->setPlaceholderText(tr("Search contact"));

	m_statusBtn = new QAction(tr("Status"), this);
	m_statusBtn->setSoftKeyRole(QAction::NegativeSoftKey);
	m_statusBtn->setMenu(statusMenu);
	addAction(m_statusBtn);

	layout->addWidget(m_searchBar);

	connect(m_searchBar, SIGNAL(textChanged(QString)), m_model, SLOT(filterList(QString)));

	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts()) {
			onAccountCreated(account);
		}
	}

	statusMenu->addAction(createGlobalStatusAction(Status::Online));
	statusMenu->addAction(createGlobalStatusAction(Status::FreeChat));
	statusMenu->addAction(createGlobalStatusAction(Status::Away));
	statusMenu->addAction(createGlobalStatusAction(Status::DND));
	statusMenu->addAction(createGlobalStatusAction(Status::NA));
	statusMenu->addAction(createGlobalStatusAction(Status::Invisible));
	statusMenu->addAction(createGlobalStatusAction(Status::Offline));

	Status status = Status(Status::Offline);
	m_statusBtn->setText(status.name());
	m_statusBtn->setProperty("lastStatus",qVariantFromValue(status));

	statusMenu->addSeparator();

	m_status_action = statusMenu->addAction(Icon("im-status-message-edit"),
											 tr("Set Status Text"),
											 this,
											 SLOT(showStatusDialog())
											 );

	QString last_status = Config().group("contactList").value("lastStatus",QString());
	m_statusBtn->setToolTip(last_status);
	m_status_action->setData(last_status);

	statusMenu->addSeparator();
	QTimer::singleShot(0, this, SLOT(init()));

	setContextMenuPolicy(Qt::ActionsContextMenu);
}


void SymbianWidget::addButton(ActionGenerator *generator)
{
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	controller->addAction(generator);
}

void SymbianWidget::removeButton(ActionGenerator *generator)
{
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	controller->removeAction(generator);
}

QAction * SymbianWidget::createGlobalStatusAction(Status::Type type)
{
	Status s = Status(type);
	QAction *act = new QAction(s.icon(), s.name(), m_statusBtn);
	connect(act, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
	act->setParent(m_statusBtn);
	act->setData(type);
	m_statusActions.append(act);
	return act;
}

bool SymbianWidget::event(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		foreach (QAction *action,m_statusActions) {
			Status last = m_statusBtn->property("lastStatus").value<Status>();
			m_statusBtn->setText(last.name());
			Status::Type type = static_cast<Status::Type>(action->data().toInt());
			action->setText(Status(type).name());
		}
		m_status_action->setText(tr("Set Status Text"));
		event->accept();
	}
	return QWidget::event(event);
}

void SymbianWidget::init()
{
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	m_actionsBtn = new QAction(tr("Actions"), this);
	m_actionsBtn->setSoftKeyRole(QAction::PositiveSoftKey);
	m_actionsBtn->setMenu(controller->menu());
	addAction(m_actionsBtn);
}

void SymbianWidget::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	//TODO add account icon
	QAction *action = new QAction(account->status().icon(), account->id(), m_statusBtn);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject*)),SLOT(onAccountDestroyed(QObject*)));
	m_actions.insert(account, action);
	action->setMenu(account->menu());
	m_statusBtn->menu()->addAction(action);
}

void SymbianWidget::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = sender_cast<Account *>(sender());
	Q_ASSERT(account);
	QAction *action = m_actions.value(account);
	Q_ASSERT(action);
	action->setIcon(status.icon());

	if (isStatusChange(status)) {
		m_statusBtn->setProperty("lastStatus",qVariantFromValue(status));
		m_statusBtn->setText(status.name());
	}
}

void SymbianWidget::onAccountDestroyed(QObject *obj)
{
	Account *account = reinterpret_cast<Account*>(obj);
	m_actions.take(account)->deleteLater();
}

void SymbianWidget::onStatusChanged()
{
	if (QAction *a = sender_cast<QAction *>(sender())) {
		Status::Type type = static_cast<Status::Type>(a->data().value<int>());
		m_statusBtn->setText(Status(type).name());
		QString text = m_status_action->data().toString();
		foreach(Account *account,Account::all()) {
			Status status = account->status();
			status.setType(type);
			status.setText(text);
			status.setProperty("changeReason",Status::ByUser);
			status.setSubtype(0);
			account->setStatus(status);
		}
	}
}

void SymbianWidget::showStatusDialog()
{
	QString text = m_status_action->data().toString();
	SimpleStatusDialog *dialog = new SimpleStatusDialog(text, this);
	connect(dialog, SIGNAL(accepted()), SLOT(changeStatusTextAccepted()));
	centerizeWidget(dialog);
	SystemIntegration::show(dialog);
}

void SymbianWidget::changeStatusTextAccepted()
{
	SimpleStatusDialog *dialog = qobject_cast<SimpleStatusDialog *>(sender());
	Q_ASSERT(dialog);
	QString text = dialog->statusText();
	m_status_action->setData(text);
	m_statusBtn->setToolTip(text);
	foreach(Protocol *proto, Protocol::all()) {
		foreach(Account *account, proto->accounts()) {
			Status status = account->status();
			status.setText(text);
			account->setStatus(status);
		}
	}
	Config config = Config().group("contactList");
	config.setValue("lastStatus",text);
	config.sync();
}

} // namespace SimpleContactList
} // namespace Core

