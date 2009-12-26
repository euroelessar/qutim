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
		JAccount *account;
		QHash<QString, JContactResource *> resources;
		QStringList currentResources;
		QSet<QString> tags;
		QString name;
		bool inList;
	};

	JContact::JContact(JAccount *account) : Contact(account), p(new JContactPrivate)
	{
		p->account = account;
	}

	JContact::~JContact()
	{
	}

	void JContact::sendMessage(const qutim_sdk_0_3::Message &message)
	{
	}

	void JContact::setName(const QString &name)
	{
		p->name = name;
	}

	QString JContact::name()
	{
		return p->name;
	}

	void JContact::setTags(const QSet<QString> &tags)
	{
		p->tags = tags;
	}

	QSet<QString> JContact::tags()
	{
		return p->tags;
	}

	bool JContact::isInList() const
	{
		return p->inList;
	}

	void JContact::setInList(bool inList)
	{
		p->inList = inList;
	}

	bool JContact::hasResource(const QString &resource)
	{
		return p->resources.contains(resource);
	}

	void JContact::addResource(const QString &resource)
	{
		JContactResource *res = new JContactResource(p->account);
		p->resources.insert(resource, res);
	}

	void JContact::setStatus(const QString &resource,
			Presence::PresenceType presence, int priority)
	{
		if (presence == Presence::Unavailable) {
			if (p->resources.contains(resource))
				removeResource(resource);
		} else {
			if (!p->resources.contains(resource))
				addResource(resource);
			p->resources.value(resource)->setStatus(presence, priority);
			fillMaxResource();
		}
	}

	void JContact::removeResource(const QString &resource)
	{
		delete p->resources.take(resource);
		fillMaxResource();
	}

	Status JContact::status()
	{
		return JProtocol::presenceToStatus(p->currentResources.isEmpty()
				? Presence::Unavailable
				: p->resources.value(p->currentResources.first())->status());
	}

	void JContact::fillMaxResource()
	{
		p->currentResources.clear();
		foreach (QString resource, p->resources.keys()) {
			if (p->currentResources.isEmpty()) {
				p->currentResources << resource;
			} else {
				int prevPriority = p->resources.value(p->currentResources.first())->priority();
				if (p->resources.value(resource)->priority() > prevPriority) {
					p->currentResources.clear();
					p->currentResources << resource;
				} else if (p->resources.value(resource)->priority() == prevPriority) {
					p->currentResources << resource;
				}
			}
		}
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
