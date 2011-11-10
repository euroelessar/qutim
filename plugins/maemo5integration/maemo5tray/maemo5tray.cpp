/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "maemo5tray.h"
#include "maemo5docklet.h"
#include <qutim/servicemanager.h>
#include <qutim/utils.h>
#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QString>

Maemo5Tray::Maemo5Tray() :
	NotificationBackend("Tray")
{
	setDescription(QT_TR_NOOP("Maemo 5 blink icon in the tray"));

	m_activeAccount = 0;
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
	
	QMap<QString, Protocol*> protocols;
	foreach (Protocol *proto, Protocol::all()) {
		protocols.insert(proto->id(), proto);
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}

	m_protocols = protocols.values();

	for (int i = 0; i < m_protocols.size(); i++) {
		foreach (Account *account, m_protocols.at(i)->accounts())
			onAccountCreated(account);
	}

	m_showIcon=true;
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
	if (notification == currentNotification())
		updateGeneratedIcon();
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
		Maemo5Docklet::SetIcon(m_currentIconPath,false);
	} else if (m_showIcon) {
		m_generatedIconPath = getIconPathForNotification(notif);
		Maemo5Docklet::SetIcon(m_generatedIconPath,true);
	}
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

static QString iconPathForStatus(const Status &status)
{
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


void Maemo5Tray::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	m_accounts << account;
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


