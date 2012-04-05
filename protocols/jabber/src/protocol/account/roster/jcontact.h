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
#ifndef JCONTACT_H
#define JCONTACT_H

#include <qutim/contact.h>
#include <jreen/presence.h>
#include "../roster/jmessagesessionowner.h"
//Jreen
#include <jreen/abstractroster.h>

namespace Jreen
{
class Presence;
}

namespace qutim_sdk_0_3
{
class InfoRequest;
}

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JContactPrivate;
class JContactResource;
class JAccount;
class JRoster;

class JContact : public Contact
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JContact)
	Q_PROPERTY(QString photoHash READ avatarHash WRITE setAvatar)
	//Q_INTERFACES(Jabber::JMessageSessionOwner)
public:
	JContact(const QString &jid, JAccount *account);
	~JContact();
	QString id() const;
	bool sendMessage(const qutim_sdk_0_3::Message &message);
	void setName(const QString &name);
	void setContactName(const QString &name);
	void setTags(const QStringList &tags);
	void setContactTags(const QStringList &tags);
	//Jreen
	void setStatus(const Jreen::Presence presence);
	void setContactSubscription(Jreen::RosterItem::SubscriptionType subscription);
	Jreen::RosterItem::SubscriptionType subscription() const;
	void setEncrypted(bool encrypted);
	bool isEncrypted() const;
	QString name() const;
	QStringList tags() const;
	Status status() const;
	bool isInList() const;
	void setInList(bool inList);
	void setContactInList(bool inList);
	bool hasResource(const QString &resource);
	void addResource(const QString &resource);
	void removeResource(const QString &resource);
	QList<JContactResource *> resources();
	JContactResource *resource(const QString &key);
	virtual ChatUnitList lowerUnits();
	QString avatar() const;
	QString avatarHash() const;
	void setAvatar(const QString &hex);
	void setExtendedInfo(const QString &name, const QVariantHash &status);
	void removeExtendedInfo(const QString &name);
	QString pgpKeyId() const;
	void setPGPKeyId(QString pgpKeyId);
	
signals:
	void subscriptionChanged(Jreen::RosterItem::SubscriptionType subscription);
	void pgpKeyChangedId(QString pgpKeyId);
	
public slots:
	void requestSubscription();
	void removeSubscription();
	
protected:
	void recalcStatus();
	void fillMaxResource();
	virtual bool event(QEvent *event);
private slots:
	void resourceStatusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
private:
	friend class JRoster;
	QScopedPointer<JContactPrivate> d_ptr;
};
}

#endif // JCONTACT_H

