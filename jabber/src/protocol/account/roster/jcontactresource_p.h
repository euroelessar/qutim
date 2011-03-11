#ifndef JCONTACTRESOURCE_P_H
#define JCONTACTRESOURCE_P_H

#include "jcontactresource.h"
#include <qutim/contact.h>
//Jreen
#include <jreen/presence.h>

namespace Jabber
{
class JContactResourcePrivate
{
public:
	JContactResourcePrivate(qutim_sdk_0_3::ChatUnit *c) :
		contact(c),
		presence(Jreen::Presence::Unavailable,Jreen::JID(c->id())) {}
	qutim_sdk_0_3::ChatUnit *contact;
	QString id;
	QString name;
	Jreen::Presence presence;
	QSet<QString> features;
	QHash<QString, QVariantHash> extInfo;
};
}

#endif // JCONTACTRESOURCE_P_H
