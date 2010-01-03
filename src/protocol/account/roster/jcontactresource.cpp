#include "jcontactresource.h"
#include "jcontact.h"
#include "../jaccount.h"
#include <QStringBuilder>

using namespace gloox;

namespace Jabber
{
	struct JResourcePrivate
	{
		JContact *contact;
		QString id;
		Presence::PresenceType presence;
		int priority;
	};

	JContactResource::JContactResource(JContact *contact, const QString &name)
			: ChatUnit(contact->account()), p(new JResourcePrivate)
	{
		p->id = contact->id() % QLatin1Char('/') % name;
	}

	JContactResource::~JContactResource()
	{
	}

	QString JContactResource::id() const
	{
		return p->id;
	}

	void JContactResource::sendMessage(const qutim_sdk_0_3::Message &message)
	{
	}

	void JContactResource::setPriority(int priority)
	{
		p->priority = priority;
	}

	int JContactResource::priority()
	{
		return p->priority;
	}

	void JContactResource::setStatus(Presence::PresenceType presence, int priority)
	{
		p->presence = presence;
		p->priority = priority;
	}

	Presence::PresenceType JContactResource::status()
	{
		return p->presence;
	}

	void JContactResource::setChatState(ChatState state)
	{
	}
}
