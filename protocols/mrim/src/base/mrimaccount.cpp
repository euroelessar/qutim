/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#include "mrimprotocol.h"
#include "roster.h"

#include "mrimaccount.h"
#include <qutim/notification.h>

struct MrimAccountPrivate
{
    MrimAccountPrivate(MrimAccount *parent)
        : conn(new MrimConnection(parent)), roster(new MrimRoster(parent))
    {
    }

    QScopedPointer<MrimConnection> conn;
    QScopedPointer<MrimRoster> roster;
	QString name;
};

MrimAccount::MrimAccount(const QString& email)
        : Account(email,MrimProtocol::instance()), p(new MrimAccountPrivate(this))
{   
    connect(connection(),SIGNAL(loggedOut()),
            roster(),SLOT(handleLoggedOut()),Qt::QueuedConnection);
    p->conn->registerPacketHandler(p->roster.data());
//    p->conn->start(); //TODO: temporary autologin, for debugging
}

MrimAccount::~MrimAccount()
{
}

QString MrimAccount::name() const
{
	return p->name.isEmpty() ? id() : p->name;
}

ChatUnit *MrimAccount::getUnit(const QString &unitId, bool create)
{
    return p->roster->getContact(unitId, create);
}

MrimConnection *MrimAccount::connection() const
{ return p->conn.data(); }

MrimRoster *MrimAccount::roster() const
{ return p->roster.data(); }

void MrimAccount::setStatus(Status status)
{
	Account::setStatus(p->conn->setStatus(status));
}

void MrimAccount::setAccountStatus(const Status &status)
{
	Account::setStatus(status);
}

void MrimAccount::setUserInfo(const QMap<QString, QString> &info)
{
	debug() << info;
	QMap<QString, QString>::const_iterator it, it2;
    it = info.find(QLatin1String("MESSAGES.TOTAL"));
    it2 = info.find(QLatin1String("MESSAGES.UNREAD"));
	if (it != info.end() && it2 != info.end()) {
		QString text = tr("Messages in mailbox: %1\nUnread messages: %2").arg(it.value(), it2.value());
		NotificationRequest request(Notification::System);
		request.setObject(this);
		request.setText(text);
		request.send();
	}
    it = info.find(QLatin1String("MRIM.NICKNAME"));
	if (it != info.end()) {
		if (p->name != it.value()) {
			QString oldName = p->name;
			p->name = it.value();
			emit nameChanged(p->name, oldName);
		}
	}
    //userInfo.userHasMyMail = info.value(QLatin1String("HAS_MYMAIL"));
    it = info.find(QLatin1String("client.endpoint"));
}

