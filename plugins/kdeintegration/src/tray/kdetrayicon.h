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

#ifndef KDETRAYICON_H
#define KDETRAYICON_H

#include <qutim/menucontroller.h>
#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/notification.h>
#include <qutim/icon.h>
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
		void onStatusChanged(Ureen::Status status);
	};
}

class KdeTrayIcon : public Ureen::MenuController, public Ureen::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
public:
    explicit KdeTrayIcon(QObject *parent = 0);
	void onActivated();
	
private slots:
	void onAccountDestroyed(QObject *obj);
	void onAccountCreated(Ureen::Account *);
	void onNotificationFinished();
	void validateIcon();
private:
	void handleNotification(Ureen::Notification *notification);
	void validateProtocolActions();
	KStatusNotifierItem *m_item;
	QMap<Ureen::Account*, Ureen::ActionGenerator*> m_actions;
	QList<KdeIntegration::ProtocolSeparatorActionGenerator*> m_protocolActions;
	QList<Ureen::Account*> m_accounts;
	QWeakPointer<Ureen::Account> m_activeAccount;
	QList<Ureen::Protocol*> m_protocols;
	QList<Ureen::Notification*> m_notifications;
	Ureen::Icon m_currentIcon;
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

