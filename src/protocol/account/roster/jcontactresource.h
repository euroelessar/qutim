#ifndef JCONTACTRESOURCE_H
#define JCONTACTRESOURCE_H

#include <qutim/chatunit.h>
#include "../jaccount.h"

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	class JAccount;

	class JContactResource : public ChatUnit
	{
	public:
		JContactResource(JAccount *account);
		QString id() const;
		void sendMessage(const qutim_sdk_0_3::Message &message);
	};
}

#endif // JCONTACTRESOURCE_H
