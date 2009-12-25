#include "jcontact.h"

namespace Jabber
{
	struct JContactPrivate
	{
		~JContactPrivate()
		{
			foreach (QString key, resources.keys())
				delete resources.take(key);
		}
		QHash<QString, JContactResource *> resources;
		QStringList currentResources;
	};

	JContact::JContact(JAccount *account) : Contact(account)
	{
	}

	JContact::~JContact()
	{
	}

	void JContact::sendMessage(const qutim_sdk_0_3::Message &message)
	{
	}

	void JContact::setName(const QString &name)
	{
	}

	void JContact::setTags(const QSet<QString> &tags)
	{
	}

	bool JContact::isInList() const
	{
		return true;
	}

	void JContact::setInList(bool inList)
	{
	}

	bool JContact::hasResource(const QString &resource)
	{
		return p->resources.contains(resource);
	}

	void JContact::addResource(const QString &resource)
	{

	}

	void JContact::setStatus(const QString &resource,
			Presence::PresenceType presence, int priority)
	{

	}

	void JContact::removeResource(const QString &resource)
	{
	}

	QStringList JContact::resources()
	{
		return p->resources.keys();
	}

	JContactResource *JContact::resource(const QString &key)
	{
		return p->resources.value(key);
	}
}
