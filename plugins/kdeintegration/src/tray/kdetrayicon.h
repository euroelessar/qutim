/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef KDETRAYICON_H
#define KDETRAYICON_H

#include <qutim/menucontroller.h>
#include <qutim/messagesession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/notification.h>
#include <KStatusNotifierItem>

namespace KdeIntegration
{
	class ProtocolSeparatorActionGenerator;
	
	class StatusAction: public QAction
	{
		Q_OBJECT
	public:
		StatusAction(QObject* parent);

	public slots:
		void onStatusChanged(qutim_sdk_0_3::Status status);
	};
}

class KdeTrayIcon : public qutim_sdk_0_3::MenuController, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::NotificationBackend)
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
public:
    explicit KdeTrayIcon(QObject *parent = 0);
	void onActivated();
	
private slots:
	void onAccountDestroyed(QObject *obj);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void onStatusChanged(const qutim_sdk_0_3::Status &);
	void onNotificationDestroyed();
private:
	void handleNotification(qutim_sdk_0_3::Notification *notification);
	QIcon convertToPixmaps(const QIcon &icon);
	void validateProtocolActions();
	KStatusNotifierItem *m_item;
	QMap<qutim_sdk_0_3::Account*, qutim_sdk_0_3::ActionGenerator*> m_actions;
	QList<KdeIntegration::ProtocolSeparatorActionGenerator*> m_protocolActions;
	QList<qutim_sdk_0_3::Account*> m_accounts;
	qutim_sdk_0_3::Account *m_activeAccount;
	QList<qutim_sdk_0_3::Protocol*> m_protocols;
	QList<qutim_sdk_0_3::Notification*> m_notifications;
	QIcon m_currentIcon;
};

class KdeStatusNotifierItem : public KStatusNotifierItem
{
public:
	KdeStatusNotifierItem(KdeTrayIcon *parent) : KStatusNotifierItem(QLatin1String("qutIM"), parent) {}
	virtual void activate(const QPoint &pos = QPoint())
	{
		Q_UNUSED(pos);
		static_cast<KdeTrayIcon*>(parent())->onActivated();
	}
};

#endif // KDETRAYICON_H
