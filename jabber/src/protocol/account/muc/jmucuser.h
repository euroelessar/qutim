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
		Q_PROPERTY(QString realJid READ realJid)
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
//			InfoRequest *infoRequest() const;
			ChatUnit *upperUnit();
			MUCRoomAffiliation affiliation();
			void setMUCAffiliation(MUCRoomAffiliation affiliation);
			MUCRoomRole role();
			void setMUCRole(MUCRoomRole role);
			QString realJid() const;
			void setRealJid(const QString &jid);
		protected:
			bool event(QEvent *ev);
			friend class JMUCSession;
	};
}

#endif // JMUCUSER_H
