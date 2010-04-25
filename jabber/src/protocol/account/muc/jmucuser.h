#ifndef JMUCUSER_H
#define JMUCUSER_H

#include "../roster/jcontactresource.h"
#include <gloox/mucroom.h>

namespace Jabber
{
	using namespace gloox;

	class JAccount;
	class JMUCSession;
	class JMUCUserPrivate;

	class JMUCUser : public JContactResource
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JMUCUser)
	public:
		JMUCUser(JMUCSession *muc, const QString &name);
		~JMUCUser();
		QString title() const;
		QString name() const;
		QString avatar() const;
		QString avatarHash() const;
		void setAvatar(const QString &hex);
		void setName(const QString &name);
		InfoRequest *infoRequest() const;
		ChatUnit *upperUnit();
		MUCRoomAffiliation affiliation();
		void setMUCAffiliation(MUCRoomAffiliation affiliation);
		MUCRoomRole role();
		void setMUCRole(MUCRoomRole role);
	};
}

#endif // JMUCUSER_H
