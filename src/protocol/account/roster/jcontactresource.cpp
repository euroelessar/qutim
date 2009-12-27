#include "jcontactresource.h"
#include "../jaccount.h"

using namespace gloox;

namespace Jabber
{
	struct JResourcePrivate
	{
		Presence::PresenceType presence;
		int priority;
	};

	JContactResource::JContactResource(JAccount *account)
			: ChatUnit(account), p(new JResourcePrivate)
	{
	}

	JContactResource::~JContactResource()
	{
	}

	QString JContactResource::id() const
	{
		return QString();
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
