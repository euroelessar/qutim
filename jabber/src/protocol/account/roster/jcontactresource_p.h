#ifndef JCONTACTRESOURCE_P_H
#define JCONTACTRESOURCE_P_H

#include "jcontactresource.h"
#include <qutim/contact.h>
//jreen
#include <jreen/presence.h>

namespace Jabber
{
class JContactResourcePrivate
{
public:
	JContactResourcePrivate(qutim_sdk_0_3::ChatUnit *c) :
		contact(c),
		presence(jreen::Presence::Unavailable,jreen::JID(c->id())) {}
	qutim_sdk_0_3::ChatUnit *contact;
	QString id;
	QString name;
	jreen::Presence presence;
	QSet<QString> features;
	QHash<QString, QVariantHash> extInfo;
	qutim_sdk_0_3::Status status;
};
}

#endif // JCONTACTRESOURCE_P_H
