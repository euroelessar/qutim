#ifndef JCONTACT_H
#define JCONTACT_H

#include <qutim/contact.h>
#include "../jaccount.h"
#include "jcontactresource.h"

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	struct JContactPrivate;
	class JContactResource;

	class JContact : public Contact
	{
		public:
			JContact(JAccount *account);
			~JContact();
			QString id() const {return QString();}
			void sendMessage(const qutim_sdk_0_3::Message &message);
			void setName(const QString &name);
			void setTags(const QSet<QString> &tags);
			void setStatus(const QString &resource, Presence::PresenceType presence, int priority);
			bool isInList() const;
			void setInList(bool inList);
			bool hasResource(const QString &resource);
			void addResource(const QString &resource);
			void removeResource(const QString &resource);
			QStringList resources();
			JContactResource *resource(const QString &key);
		private:
			QScopedPointer<JContactPrivate> p;
	};
}

#endif // JCONTACT_H
