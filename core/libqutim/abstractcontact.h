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

#ifndef ABSTRACTCONTACT_H
#define ABSTRACTCONTACT_H

#include "chatunit.h"
#include "status.h"
#include <QIcon>
#include <QScopedPointer>

#ifndef Q_QDOC
namespace qutim_sdk_0_3
{
struct AbstractContactPrivate;
class MetaContact;
class Message;

//Remove me
class LIBQUTIM_EXPORT AbstractContact : public ChatUnit
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(Status status READ status NOTIFY statusChanged)
public:
	AbstractContact(const QString &id, Account *parent = 0);
	virtual ~AbstractContact();
	QString id();
	virtual QString name() const;
	virtual Status status() const;
	MetaContact *metaContact();
	virtual bool sendMessage(const Message &message) = 0;
	virtual void setName(const QString &name) = 0;
signals:
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
	void nameChanged(const QString &current, const QString &previous);
private:
	QScopedPointer<AbstractContactPrivate> p;
};
}
#endif

#endif // ABSTRACTCONTACT_H

