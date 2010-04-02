#ifndef JCONTACTRESOURCE_P_H
#define JCONTACTRESOURCE_P_H

#include <qutim/contact.h>
#include <gloox/presence.h>

namespace Jabber
{
	class JContactResourcePrivate
	{
	public:
		JContactResourcePrivate() :
				contact(0), presence(gloox::Presence::Unavailable), priority(0) {}
		qutim_sdk_0_3::ChatUnit *contact;
		QString id;
		QString name;
		gloox::Presence::PresenceType presence;
		int priority;
		QSet<QString> features;
		QString text;
	};
}

#endif // JCONTACTRESOURCE_P_H
