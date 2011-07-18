#ifndef JCONTACTRESOURCE_P_H
#define JCONTACTRESOURCE_P_H

#include "jcontactresource.h"
#include <qutim/contact.h>
#include "../jaccount.h"
//Jreen
#include <jreen/presence.h>

namespace Jabber
{
class JContactResourcePrivate
{
public:
	JContactResourcePrivate(qutim_sdk_0_3::ChatUnit *c) :
		contact(c),
		presence(Jreen::Presence::Unavailable,Jreen::JID(c->id())),
		isAccountResource(false)
	{}
	JContactResourcePrivate(JAccount *a) :
		account(a),
		presence(Jreen::Presence::Unavailable,Jreen::JID(a->id())),
		isAccountResource(true)
	{}
	union {
		qutim_sdk_0_3::ChatUnit *contact;
		JAccount *account;
	};
	QString id;
	QString name;
	Jreen::Presence presence;
	QSet<QString> features;
	QHash<QString, QVariantHash> extInfo;
	bool isAccountResource;
};
}

#endif // JCONTACTRESOURCE_P_H
