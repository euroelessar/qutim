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
#ifndef JMUCUSER_H
#define JMUCUSER_H

#include "../roster/jcontactresource.h"
#include <jreen/mucroom.h>

namespace Jabber
{
using namespace Jreen;

class JAccount;
class JMUCSession;
class JMUCUserPrivate;

class JMUCUser : public JContactResource
{
	Q_OBJECT
	Q_PROPERTY(QString realJid READ realJid)
	Q_PROPERTY(QString photoHash READ avatarHash WRITE setAvatar)
	Q_PROPERTY(int affiliation READ affiliation NOTIFY affiliationChanged)
	Q_PROPERTY(int mucRole READ mucRole NOTIFY mucRoleChanged)
	Q_PROPERTY(int priority READ priority NOTIFY priorityChanged)
	Q_DECLARE_PRIVATE(JMUCUser)
public:
	JMUCUser(JMUCSession *muc, const QString &name);
	~JMUCUser();
	QString title() const;
	QString name() const;
	void setUserName(const QString &name);
	QString avatar() const;
	QString avatarHash() const;
	int affiliation() const;
	int mucRole() const;
	int priority() const;
	void setAvatar(const QString &hex);
	void setName(const QString &name);
	//			InfoRequest *infoRequest() const;
	JMUCSession *muc() const;
	ChatUnit *upperUnit();
	MUCRoom::Affiliation affiliation();
	void setMUCAffiliationAndRole(MUCRoom::Affiliation affiliation, MUCRoom::Role role);
	MUCRoom::Role role();
	QString realJid() const;
	void setRealJid(const QString &jid);
	bool sendMessage(const qutim_sdk_0_3::Message &message);
public slots:
	void kick(const QString &reason = QString());
	void ban(const QString &reason = QString());
protected:
	bool event(QEvent *ev);
	friend class JMUCSession;
signals:
	void affiliationChanged(const int &affiliation);
	void mucRoleChanged(const int &mucRole);
	void priorityChanged(const int &oldPriority, const int &newPriority);
};
}

#endif // JMUCUSER_H

