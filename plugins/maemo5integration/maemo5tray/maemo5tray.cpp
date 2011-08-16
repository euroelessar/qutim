/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#include "maemo5tray.h"
#include "maemo5docklet.h"
#include <qutim/extensioninfo.h>
#include "qutim/metaobjectbuilder.h"
#include <qutim/servicemanager.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/utils.h>
#include <QApplication>
#include <QDateTime>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QToolButton>
#include <QWidget>
#include <QWidgetAction>

class ProtocolSeparatorActionGenerator : public ActionGenerator
{
public:
	ProtocolSeparatorActionGenerator(Protocol *proto, const ExtensionInfo &info);
	virtual QObject *generateHelper() const;
	virtual ~ProtocolSeparatorActionGenerator();
	void ensureVisibility() const;
	void showImpl(QAction *, QObject *);
private:
	Protocol *m_proto;
	QToolButton *m_btn;
	mutable QWeakPointer<QAction> m_action;
};

Maemo5Tray::Maemo5Tray() :
	NotificationBackend("Tray")
{
	setDescription(QT_TR_NOOP("Maemo 5 blink icon in the tray"));

	m_activeAccount = 0;
	m_showGeneratedIcon = false;

	//m_icon->setIcon(m_currentIcon = Icon("qutim-offline"));
	m_offlineIconPath	      = IconLoader::iconPath("qutim-offline",Maemo5Docklet::GetIconSize());
	m_mailIconPath                = IconLoader::iconPath("mail-message-new-qutim",Maemo5Docklet::GetIconSize());
	m_typingIconPath              = IconLoader::iconPath("im-status-message-edit",Maemo5Docklet::GetIconSize());
	m_chatUserJoinedIconPath      = IconLoader::iconPath("list-add-user-conference",Maemo5Docklet::GetIconSize());
	m_chatUserLeftIconPath        = IconLoader::iconPath("list-remove-user-conference",Maemo5Docklet::GetIconSize());
	m_qutimIconPath               = IconLoader::iconPath("qutim",Maemo5Docklet::GetIconSize());
	m_transferCompletedIconPath   = IconLoader::iconPath("mdocument-save-filetransfer-comleted",Maemo5Docklet::GetIconSize());
	m_defaultNotificationIconPath = IconLoader::iconPath("dialog-information",Maemo5Docklet::GetIconSize());



	Maemo5Docklet::Enable();
	Maemo5Docklet::SetTooltip("qutIM::Offline");
	Maemo5Docklet::SetIcon(m_offlineIconPath,false);


	connect(Maemo5Docklet::Instance(), SIGNAL(ButtonClicked()),this, SLOT(onActivated()));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));


	QMap<QString, Protocol*> protocols;
	foreach (Protocol *proto, Protocol::all()) {
		protocols.insert(proto->id(), proto);
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}
	m_protocols = protocols.values();

	for (int i = 0; i < m_protocols.size(); i++) {
		ExtensionInfo info = m_protocols.at(i)->property("extensioninfo").value<ExtensionInfo>();
		ProtocolSeparatorActionGenerator *gen =
				new ProtocolSeparatorActionGenerator(m_protocols.at(i), info);
		m_protocolActions.append(gen);
		gen->setPriority(1 - i * 2);
		addAction(gen);
		foreach (Account *account, m_protocols.at(i)->accounts())
			onAccountCreated(account);
	}
	m_showIcon=true;
	setMenuOwner(qobject_cast<MenuController*>(ServiceManager::getByName("ContactList")));

	qApp->setQuitOnLastWindowClosed(false);




}

Maemo5Tray::~Maemo5Tray()
{
	Maemo5Docklet::Disable();
}

void Maemo5Tray::onActivated()
{
	Notification *notif = currentNotification();
	if (!notif) {
		if (QObject *obj = ServiceManager::getByName("ContactList")) {
			obj->metaObject()->invokeMethod(obj, "changeVisibility");
		}
	} else {
		notif->accept();
	}

}

void Maemo5Tray::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
	connect(session, SIGNAL(destroyed()), this, SLOT(onSessionDestroyed()));
}

void Maemo5Tray::onSessionDestroyed()
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	m_sessions.remove(session);
	updateGeneratedIcon();
}

void Maemo5Tray::onUnreadChanged(qutim_sdk_0_3::MessageList unread)
{
	ChatSession *session = sender_cast<ChatSession*>(sender());

	MessageList::iterator itr = unread.begin();
	while (itr != unread.end()) {
		if (itr->property("silent", false))
			itr = unread.erase(itr);
		else
			++itr;
	}

	if (unread.isEmpty())
		m_sessions.remove(session);
	else
		m_sessions.insert(session, unread.count());

	updateGeneratedIcon();
}

void Maemo5Tray::onNotificationFinished()
{
	Notification *notif = sender_cast<Notification*>(sender());
	deref(notif);
	Notification *current = currentNotification();
	if (!m_messageNotifications.removeOne(notif))
		if (!m_notifications.removeOne(notif))
			m_typingNotifications.removeOne(notif);
	if (current == notif)
		updateGeneratedIcon();
}


void Maemo5Tray::handleNotification(Notification *notification)
{
	Notification::Type type = notification->request().type();

	if (type == Notification::IncomingMessage ||
		type == Notification::OutgoingMessage ||
		type == Notification::ChatIncomingMessage ||
		type == Notification::ChatOutgoingMessage)
	{
		m_messageNotifications << notification;
	} else if (type == Notification::UserTyping) {
		m_typingNotifications << notification;
	} else {
		m_notifications << notification;
	}

	ref(notification);
	connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
			SLOT(onNotificationFinished()));

	if (!m_iconTimer.isActive() && m_blink && m_showIcon) {
		m_iconTimer.start(500, this);
		m_showGeneratedIcon = true;
	}
	if (notification == currentNotification())
		updateGeneratedIcon();
}

void Maemo5Tray::timerEvent(QTimerEvent *timer)
{
	if (timer->timerId() != m_iconTimer.timerId()) {
		QObject::timerEvent(timer);
	} else {
		Maemo5Docklet::SetIcon(m_showGeneratedIcon ? m_generatedIconPath : m_currentIconPath,false);
		m_showGeneratedIcon = !m_showGeneratedIcon;
	}
}


QString Maemo5Tray::getIconPathForNotification(Notification *notification)
{
	switch (notification->request().type()) {
	case Notification::IncomingMessage:
	case Notification::OutgoingMessage:
	case Notification::ChatIncomingMessage:
	case Notification::ChatOutgoingMessage:
		return m_mailIconPath;
	case Notification::UserTyping:
		return m_typingIconPath;
	case Notification::ChatUserJoined:
		return m_chatUserJoinedIconPath;
	case Notification::ChatUserLeft:
		return m_chatUserLeftIconPath;
	case Notification::AppStartup:
		return m_qutimIconPath;
	case Notification::FileTransferCompleted:
		return m_transferCompletedIconPath;
	case Notification::UserOnline:
	case Notification::UserOffline:
	case Notification::UserChangedStatus:
	case Notification::BlockedMessage:
	case Notification::UserHasBirthday:
	case Notification::System:
		return m_defaultNotificationIconPath;
	}
	return QString();
}


void Maemo5Tray::updateGeneratedIcon()
{
	Notification *notif = currentNotification();
	if (!notif) {
		if (m_iconTimer.isActive())
			m_iconTimer.stop();
		Maemo5Docklet::SetIcon(m_currentIconPath,false);
		m_showGeneratedIcon = false;
	} else if (m_showIcon) {

		m_generatedIconPath = getIconPathForNotification(notif);

		if (!m_blink || m_showGeneratedIcon) {

			m_showGeneratedIcon = true;
		}
		Maemo5Docklet::SetIcon(m_generatedIconPath,true);
	}
	qDebug()<<"updateGeneratedIcon";
}

Notification *Maemo5Tray::currentNotification()
{
	// Message notifications have highest priority
	if (!m_messageNotifications.isEmpty())
		return m_messageNotifications.first();

	else if (!m_notifications.isEmpty())
		return m_notifications.first();

	// Typing notifications have lowest priority
	else if (!m_typingNotifications.isEmpty())
		return m_typingNotifications.first();

	return 0;
}


//---------------------------------------------------------------------------
// Accounts context menu implementation
//---------------------------------------------------------------------------

class AccountMenuActionGenerator : public ActionGenerator
{
public:
	AccountMenuActionGenerator(Account *account) :
		ActionGenerator(QIcon(), account->id(), 0, 0)
	{
		m_account = account;
		setType(-1);
	}

	virtual QObject *generateHelper() const
	{
		StatusAction *action = new StatusAction(0);
		prepareAction(action);
		action->setIcon(m_account->status().icon());
		QMenu *menu = m_account->menu(false);
		QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
		QObject::connect(m_account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
						 action, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
		action->setMenu(menu);
		return action;
	}
private:
	Account *m_account;
};

static QString iconPathForStatus(const Status &status)
{
	//TODO create icon
	if (status.type() == Status::Offline){
		Maemo5Docklet::SetTooltip("qutIM::Offline");
		return IconLoader::iconPath("qutim-offline",Maemo5Docklet::GetIconSize());
		}
	else if (status.type() == Status::Connecting){
		Maemo5Docklet::SetTooltip("qutIM::Offline");
		return IconLoader::iconPath("qutim-offline",Maemo5Docklet::GetIconSize());
		}else{
		Maemo5Docklet::SetTooltip("qutIM::Online");
		return IconLoader::iconPath("qutim-online",Maemo5Docklet::GetIconSize());
		}
}

ProtocolSeparatorActionGenerator::ProtocolSeparatorActionGenerator(Protocol *proto, const ExtensionInfo &info) :
	ActionGenerator(info.icon(), MetaObjectBuilder::info(info.generator()->metaObject(),"Protocol"), 0, 0)
{
	setType(-1);
	m_proto = proto;
}

QObject *ProtocolSeparatorActionGenerator::generateHelper() const
{
	if (m_action)
		return m_action.data();
	QAction *action = prepareAction(new QAction(NULL));
	QFont font = action->font();
	font.setBold(true);
	action->setFont(font);
	QToolButton *m_btn = new QToolButton();
	QWidgetAction *widget = new QWidgetAction(action);
	m_action = QWeakPointer<QAction>(widget);
	widget->setDefaultWidget(m_btn);
	QObject::connect(widget, SIGNAL(destroyed()), action, SLOT(deleteLater()));
	QObject::connect(widget, SIGNAL(destroyed()), m_btn, SLOT(deleteLater()));
	m_btn->setDefaultAction(action);
	m_btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_btn->setDown(true); // prevent hover style changes in some styles
	return widget;

}

ProtocolSeparatorActionGenerator::~ProtocolSeparatorActionGenerator()
{
}

void ProtocolSeparatorActionGenerator::ensureVisibility() const
{
	if (m_action)
		m_action.data()->setVisible(!m_proto->accounts().isEmpty());
}

void ProtocolSeparatorActionGenerator::showImpl(QAction *, QObject *)
{
	ensureVisibility();
}


StatusAction::StatusAction(QObject* parent): QAction(parent)
{

}

void StatusAction::onStatusChanged(Status status)
{
	setIcon(status.icon());
}

void Maemo5Tray::onAccountDestroyed(QObject *obj)
{
	ActionGenerator *gen = m_actions.take(static_cast<Account*>(obj));
	removeAction(gen);
	delete gen;

}

void Maemo5Tray::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	if (m_actions.contains(account))
		return;
	m_accounts << account;
	ActionGenerator *gen = new AccountMenuActionGenerator(account);
	gen->setPriority(- m_protocols.indexOf(account->protocol()) * 2);
	m_actions.insert(account, gen);
	addAction(gen);
	connect(account, SIGNAL(destroyed(QObject*)), this, SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));

}

void Maemo5Tray::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = qobject_cast<Account*>(sender());
	if (account == m_activeAccount || !m_activeAccount) {
		m_activeAccount = account;
		if (account->status().type() == Status::Offline) {
			m_activeAccount = 0;
		}
		m_currentIconPath = iconPathForStatus(status);
	}
	if (!m_activeAccount) {
		foreach (Account *acc, m_accounts) {
			if (acc->status().type() != Status::Offline) {
				m_activeAccount = acc;
				m_currentIconPath = iconPathForStatus(acc->status());
				break;
			}
		}
	}

	Maemo5Docklet::SetIcon(m_currentIconPath,false);

}


