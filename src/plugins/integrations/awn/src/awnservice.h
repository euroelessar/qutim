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
#ifndef AWNSERVICE
#define AWNSERVICE

#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>

#include "chatwindowcontroller.h"

#include <Qt/QtDBus>

using namespace qutim_sdk_0_3;

class AWNService : public MenuController
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	explicit AWNService();
	virtual ~AWNService();
private:
	int addDockMenu(QString caption, QString icon_name = QString(), QString parent = QString());
	void removeDockMenu(int id);
	ChatWindowController *m_cwc;
	QDBusInterface *m_awn;
	QDBusInterface *m_item;
	QTimer *m_iconTimer;
	QList<ChatSession*> m_sessions;
	QList<int> m_menus;
	QList<Account*> m_accounts;
	QStringList m_capabilities;
	QString m_currentIcon;
	QString m_firstIcon;
	QString m_secondIcon;
	Account *m_activeAccount;
	int m_icon_size;
	int m_showhide_menu;
	int m_online_menu;
	int m_offline_menu;
	int m_readall_menu;
	int m_exit_menu;
	int m_uread;
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onUnreadChanged(const qutim_sdk_0_3::MessageList&);
	void onStatusChanged(const qutim_sdk_0_3::Status &status);
	void onItemRemoved(QDBusObjectPath path);
	void onMenuItemActivated(int);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void onAccountDestroyed(QObject *obj);

	void setDockIcon(QString icon_name);
	void setDockText(QString text);
	void setItem(QDBusObjectPath item_path);

	void nextIcon();
	void generateIcons();
	QString generateIcon(QIcon icon, QString icon_name = QString());
};

#endif //AWNService

