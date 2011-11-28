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
#ifndef JCONTACTRESOURCE_H
#define JCONTACTRESOURCE_H

#include <qutim/buddy.h>
#include <jreen/presence.h>
#include <QSet>
#include "../roster/jmessagesessionowner.h"

namespace Jreen
{
class Presence;
}

namespace qutim_sdk_0_3
{
class Status;
class InfoRequest;
}

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JAccount;
class JContact;
class JContactResourcePrivate;

class JContactResource : public qutim_sdk_0_3::Buddy
{
	Q_PROPERTY(QSet<QString> features READ features WRITE setFeatures)
	Q_DECLARE_PRIVATE(JContactResource)
	Q_OBJECT
	//Q_INTERFACES(Jabber::JMessageSessionOwner)
public:
	JContactResource(qutim_sdk_0_3::ChatUnit *parent, const QString &name);
	JContactResource(qutim_sdk_0_3::ChatUnit *parent, JContactResourcePrivate &ptr);
	~JContactResource();
	QString name() const;
	QString id() const;
	QString title() const;
	bool sendMessage(const qutim_sdk_0_3::Message &message);
	void setPriority(int priority);
	int priority();
	void setStatus(const Jreen::Presence presence);
	Status status() const;
	virtual bool event(QEvent *ev);
	QSet<QString> features() const;
	void setFeatures(const QSet<QString> &features);
	bool checkFeature(const QLatin1String &feature) const;
	bool checkFeature(const QString &feature) const;
	bool checkFeature(const std::string &feature) const;
	ChatUnit *upperUnit();
	QString avatar() const;
	QString text() const;
	void setExtendedInfo(const QString &name, const QVariantHash &info);
	void removeExtendedInfo(const QString &name);
protected:
	QScopedPointer<JContactResourcePrivate> d_ptr;
};
}

Q_DECLARE_METATYPE(QSet<QString>)

#endif // JCONTACTRESOURCE_H

