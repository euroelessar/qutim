#ifndef JCONTACTRESOURCE_H
#define JCONTACTRESOURCE_H

#include <qutim/contact.h>
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

class JContactResource : public qutim_sdk_0_3::Contact
{
	Q_PROPERTY(QSet<QString> features READ features WRITE setFeatures)
	Q_DECLARE_PRIVATE(JContactResource)
	Q_OBJECT
	//Q_INTERFACES(Jabber::JMessageSessionOwner)
public:
	JContactResource(qutim_sdk_0_3::ChatUnit *parent, const QString &name);
	JContactResource(qutim_sdk_0_3::ChatUnit *parent, JContactResourcePrivate &ptr);
	JContactResource(JAccount *parent, const QString &resource);
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

	QStringList tags() const;
	void setTags(const QStringList &tags);
	bool isInList() const;
	void setInList(bool inList);
protected:
	QScopedPointer<JContactResourcePrivate> d_ptr;
};
}

Q_DECLARE_METATYPE(QSet<QString>)

#endif // JCONTACTRESOURCE_H
