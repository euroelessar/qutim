#include "jcontact_p.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include <gloox/message.h>
#include <gloox/chatstate.h>
#include <gloox/rostermanager.h>
#include <gloox/rosteritem.h>

using namespace gloox;

namespace Jabber
{
	JContact::JContact(const QString &jid, JAccount *account) : Contact(account), d_ptr(new JContactPrivate)
	{
		Q_D(JContact);
		d->account = account;
		d->jid = jid;
	}

	JContact::~JContact()
	{
	}

	void JContact::sendMessage(const qutim_sdk_0_3::Message &message)
	{
	}

	void JContact::setName(const QString &name)
	{
		Q_D(JContact);
		RosterManager *rosterManager = d->account->connection()->client()->rosterManager();
		RosterItem *item = rosterManager->getRosterItem(d->jid.toStdString());
		if (!item)
			return;
		item->setName(name.toStdString());
		rosterManager->synchronize();
	}

	QString JContact::name()
	{
		return d_func()->name;
	}

	void JContact::setTags(const QSet<QString> &tags)
	{
		d_func()->tags = tags;
	}

	QSet<QString> JContact::tags()
	{
		return d_func()->tags;
	}

	bool JContact::isInList() const
	{
		return d_func()->inList;
	}

	void JContact::setInList(bool inList)
	{
		Q_D(JContact);
		if (d->inList == inList)
			return;
		RosterManager *rosterManager = d->account->connection()->client()->rosterManager();
		if (inList)
			rosterManager->add(d->jid.toStdString(), d->name.toStdString(), StringList());
		else
			rosterManager->remove(d->jid.toStdString());
	}

	inline gloox::ChatStateType qutimToGloox(qutim_sdk_0_3::ChatState state)
	{
		switch (state) {
		case qutim_sdk_0_3::ChatStateActive:
			return gloox::ChatStateActive;
		case qutim_sdk_0_3::ChatStateInActive:
			return gloox::ChatStateInactive;
		case qutim_sdk_0_3::ChatStateGone:
			return gloox::ChatStateGone;
		case qutim_sdk_0_3::ChatStateComposing:
			return gloox::ChatStateComposing;
		case qutim_sdk_0_3::ChatStatePaused:
			return gloox::ChatStatePaused;
		default:
			return gloox::ChatStateInvalid;
		}
	}

	void JContact::setChatState(qutim_sdk_0_3::ChatState state)
	{
		Q_D(JContact);
		Client *client = d->account->connection()->client();
		gloox::Message gmes(gloox::Message::Chat, d->jid.toStdString());
		gmes.addExtension(new gloox::ChatState(qutimToGloox(state)));
		client->send(gmes);
	}

	bool JContact::hasResource(const QString &resource)
	{
		return d_func()->resources.contains(resource);
	}

	void JContact::addResource(const QString &resource)
	{
		JContactResource *res = new JContactResource(d_func()->account);
		d_func()->resources.insert(resource, res);
	}

	void JContact::setStatus(const QString &resource, Presence::PresenceType presence, int priority)
	{
		Q_D(JContact);
		if (presence == Presence::Unavailable) {
			if (d->resources.contains(resource))
				removeResource(resource);
		} else {
			if (!d->resources.contains(resource))
				addResource(resource);
			d->resources.value(resource)->setStatus(presence, priority);
			fillMaxResource();
		}
	}

	void JContact::removeResource(const QString &resource)
	{
		delete d_func()->resources.take(resource);
		fillMaxResource();
	}

	Status JContact::status()
	{
		Q_D(JContact);
		return JProtocol::presenceToStatus(d->currentResources.isEmpty()
				? Presence::Unavailable
				: d->resources.value(d->currentResources.first())->status());
	}

	void JContact::fillMaxResource()
	{
		Q_D(JContact);
		d->currentResources.clear();
		foreach (QString resource, d->resources.keys()) {
			if (d->currentResources.isEmpty()) {
				d->currentResources << resource;
			} else {
				int prevPriority = d->resources.value(d->currentResources.first())->priority();
				if (d->resources.value(resource)->priority() > prevPriority) {
					d->currentResources.clear();
					d->currentResources << resource;
				} else if (d->resources.value(resource)->priority() == prevPriority) {
					d->currentResources << resource;
				}
			}
		}
	}

	QStringList JContact::resources()
	{
		return d_func()->resources.keys();
	}

	JContactResource *JContact::resource(const QString &key)
	{
		return d_func()->resources.value(key);
	}
}
