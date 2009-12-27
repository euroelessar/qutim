#ifndef JCONTACTRESOURCE_H
#define JCONTACTRESOURCE_H

#include <qutim/chatunit.h>
#include <gloox/presence.h>

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	class JAccount;
	struct JResourcePrivate;

	class JContactResource : public ChatUnit
	{
		public:
			JContactResource(JAccount *account);
			~JContactResource();
			QString id() const;
			void sendMessage(const qutim_sdk_0_3::Message &message);
			void setPriority(int priority);
			int priority();
			void setStatus(gloox::Presence::PresenceType presence, int priority);
			gloox::Presence::PresenceType status();
			void setChatState(ChatState state);
		private:
			QScopedPointer<JResourcePrivate> p;
	};
}

#endif // JCONTACTRESOURCE_H
