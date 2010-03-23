#ifndef JCONTACT_H
#define JCONTACT_H

#include <qutim/contact.h>
#include "jcontactresource.h"

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	struct JContactPrivate;
	class JContactResource;
	class JAccount;
	class JRoster;

	class JContact : public Contact
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JContact)
		public:
			JContact(const QString &jid, JAccount *account);
			~JContact();
			QString id() const;
			void sendMessage(const qutim_sdk_0_3::Message &message);
			void setName(const QString &name);
			void setTags(const QSet<QString> &tags);
			void setStatus(const QString &resource, gloox::Presence::PresenceType presence, int priority);
			QString name() const;
			QSet<QString> tags() const;
			Status status() const;
			bool isInList() const;
			void setInList(bool inList);
			bool hasResource(const QString &resource);
			void addResource(const QString &resource);
			void removeResource(const QString &resource);
			QStringList resources();
			JContactResource *resource(const QString &key);
			ChatUnitList lowerUnits();
			virtual bool event(QEvent *);
			QString avatar() const;
			QString avatarHash() const;
			void setAvatar(const QString &hex);
		protected:
			void fillMaxResource();
		private:
			friend class JRoster;
			QScopedPointer<JContactPrivate> d_ptr;
	};
}

#endif // JCONTACT_H
