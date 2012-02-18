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
#ifndef CONFERENCE_H
#define CONFERENCE_H

#include "chatunit.h"
#include "buddy.h"

namespace qutim_sdk_0_3
{
class ConferencePrivate;
class Buddy;

class LIBQUTIM_EXPORT Conference : public ChatUnit
{
	Q_DECLARE_PRIVATE(Conference)
	Q_PROPERTY(QString topic READ topic WRITE setTopic NOTIFY topicChanged)
	Q_PROPERTY(qutim_sdk_0_3::Buddy* me READ me NOTIFY meChanged)
	Q_PROPERTY(bool isJoined READ isJoined NOTIFY joinedChanged)
	Q_OBJECT
public:
	Conference(Account *account);
	virtual ~Conference();
	virtual QString topic() const;
	virtual void setTopic(const QString &topic);
	virtual Buddy *me() const = 0;
	bool isJoined() const;
public slots:
	void join();
	void leave();
	virtual void invite(qutim_sdk_0_3::Contact *contact, const QString &reason = QString());
signals:
	void topicChanged(const QString &current, const QString &previous);
	void meChanged(qutim_sdk_0_3::Buddy *me);
	void left();
	void joined();
	void joinedChanged(bool isJoined);
protected slots:
	void setJoined(bool set);
protected:
	bool event(QEvent *ev);
	virtual void doJoin() = 0;
	virtual void doLeave() = 0;
};

}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Conference*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::Conference*>)

#endif // CONFERENCE_H

