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
signals:
	void subscriptionChanged(Jreen::RosterItem::SubscriptionType subscription);
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
