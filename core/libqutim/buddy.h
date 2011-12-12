/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef BUDDY_H
#define BUDDY_H

#include "chatunit.h"
#include "status.h"
#include <QIcon>
#include <QMetaType>

namespace qutim_sdk_0_3
{
class Account;
class Message;
class BuddyPrivate;

class LIBQUTIM_EXPORT Buddy : public ChatUnit
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(Buddy)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)
	Q_PROPERTY(qutim_sdk_0_3::Status status READ status NOTIFY statusChanged)
public:
	Buddy(Account *account);
	Buddy(BuddyPrivate &d, Account *account);
	virtual ~Buddy();
	/*!
	Path to buddy image
  */
	virtual QString avatar() const;
	/**
  * @brief Returns contact's representable name
  *
  * @return Contact's name
  */
	virtual QString title() const;
	virtual QString name() const;
	virtual Status status() const;
	/**
  * @brief send message to contact
  *
  * @param message Message, which to be sent to the recipient
  */
	virtual bool sendMessage(const Message &message) = 0;
	virtual void setName(const QString &name);
//	virtual	void setStatus(const Status &);
signals:
	void avatarChanged(const QString &path);
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
	void nameChanged(const QString &current, const QString &previous);
protected:
	bool event(QEvent *ev);
private:
	//TODO move to protocols
	Q_PRIVATE_SLOT(d_func(), void _q_status_changed(const qutim_sdk_0_3::Status &now,const qutim_sdk_0_3::Status &old))
};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Buddy*)
Q_DECLARE_METATYPE(QList<qutim_sdk_0_3::Buddy*>)

#endif // BUDDY_H

