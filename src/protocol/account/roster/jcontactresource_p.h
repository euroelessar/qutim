#ifndef JCONTACTRESOURCE_P_H
#define JCONTACTRESOURCE_P_H

#include "jcontactresource.h"

namespace Jabber
{
	class JContactResourcePrivate
	{
	public:
		qutim_sdk_0_3::ChatUnit *contact;
		QString id;
		QString name;
		gloox::Presence::PresenceType presence;
		int priority;
		QSet<QString> features;
	};
}

#endif // JCONTACTRESOURCE_P_H
