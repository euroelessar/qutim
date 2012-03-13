/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef MACDOCK_H
#define MACDOCK_H

#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QSystemTrayIcon>
#include <QBasicTimer>

namespace MacIntegration
{
using namespace qutim_sdk_0_3;

class MacDockPrivate;
class MacDock : public MenuController
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_DECLARE_PRIVATE(MacDock)
public:
	MacDock();
	~MacDock();
	void dockIconClickEvent();
	void loadSettings();
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onSessionDestroyed();
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void setStatusIcon();
	void onStatusChanged();
	void onActivatedSession(bool state);
	void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
protected:
	void createDockDeps();
	void removeTrayDeps();
	void createStatusAction(Status::Type type, QMenu *parent);
	void createStatuses(QMenu *parent);
	void createTrayDeps();
	void setBadgeLabel(const QString &string);
	virtual void timerEvent(QTimerEvent *timer);
private:
	QScopedPointer<MacDockPrivate> d_ptr;
};
}

#endif // MACDOCK_H

