#include "jcontactresource_p.h"
#include "jcontact.h"
#include "../jaccount.h"
#include "../../jprotocol.h"
#include <qutim/status.h>
#include <QStringBuilder>

using namespace gloox;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	JContactResource::JContactResource(ChatUnit *parent, const QString &name) :
			Buddy(parent->account()), d_ptr(new JContactResourcePrivate)
	{
		Q_D(JContactResource);
		d->name = name;
		d->id = parent->id() % QLatin1Char('/') % name;
		connect(parent, SIGNAL(avatarChanged(QString)), this, SIGNAL(avatarChanged(QString)));
	}

	JContactResource::JContactResource(ChatUnit *parent, JContactResourcePrivate &ptr) :
			Buddy(parent->account()), d_ptr(&ptr)
	{
	}

	JContactResource::~JContactResource()
	{
	}

	QString JContactResource::id() const
	{
		return d_func()->id;
	}

	QString JContactResource::title() const
	{
		Q_D(const JContactResource);
		if (Contact *contact = qobject_cast<Contact *>(d->contact)) {
			return contact->title() % "/" % d->name;
		} else {
			return ChatUnit::title();
		}
	}

	void JContactResource::sendMessage(const qutim_sdk_0_3::Message &message)
	{
	}

	void JContactResource::setPriority(int priority)
	{
		d_func()->priority = priority;
	}

	int JContactResource::priority()
	{
		return d_func()->priority;
	}

	void JContactResource::setStatus(Presence::PresenceType presence, int priority, const QString &text)
	{
		Q_D(JContactResource);
		d->presence = presence;
		d->priority = priority;
		d->text = text;
		emit statusChanged(JProtocol::presenceToStatus(presence));
	}

	Status JContactResource::status() const
	{
		return JProtocol::presenceToStatus(d_func()->presence);
	}

	bool JContactResource::event(QEvent *ev)
	{
		if (ev->type() == ChatStateEvent::eventType()) {
//			Q_D(JContactResource);
//			ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
			//TODO
			//Client *client = d->contact->account->connection()->client();
			//gloox::Message gmes(gloox::Message::Chat, d->jid.toStdString());
			//gmes.addExtension(new gloox::ChatState(qutIM2gloox(chatEvent->chatState())));
			//client->send(gmes);
		}
		return ChatUnit::event(ev);
	}

	QSet<QString> JContactResource::features() const
	{
		return d_ptr->features;
	}

	void JContactResource::setFeatures(const QSet<QString> &features)
	{
		d_ptr->features = features;
	}

	bool JContactResource::checkFeature(const QLatin1String &feature) const
	{
		return d_ptr->features.contains(feature);
	}

	bool JContactResource::checkFeature(const QString &feature) const
	{
		return d_ptr->features.contains(feature);
	}

	bool JContactResource::checkFeature(const std::string &feature) const
	{
		return d_ptr->features.contains(QString::fromStdString(feature));
	}

	ChatUnit *JContactResource::upperUnit()
	{
		return qobject_cast<Contact *>(d_ptr->contact);
	}

	QString JContactResource::avatar() const
	{
		return qobject_cast<JContact *>(d_func()->contact)->avatar();
	}

	QString JContactResource::text() const
	{
		return d_func()->text;
	}
}
