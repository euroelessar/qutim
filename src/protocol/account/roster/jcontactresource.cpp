#include "jcontactresource.h"

namespace Jabber
{
	JContactResource::JContactResource(JAccount *account) : ChatUnit(account)
	{
	}

	QString JContactResource::id() const
	{
		return QString();
	}

	void JContactResource::sendMessage(const qutim_sdk_0_3::Message &message)
	{

	}
}
