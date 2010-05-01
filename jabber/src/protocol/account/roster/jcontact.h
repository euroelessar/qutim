#ifndef JCONTACT_H
#define JCONTACT_H

#include <qutim/contact.h>
#include <gloox/presence.h>

namespace qutim_sdk_0_3
{
	class InfoRequest;
}

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
			void setContactName(const QString &name);
			void setTags(const QSet<QString> &tags);
			void setContactTags(const QSet<QString> &tags);
			void setStatus(const QString &resource, gloox::Presence::PresenceType presence, int priority,
					const QString &text = QString());
			QString name() const;
			QSet<QString> tags() const;
			Status status() const;
			bool isInList() const;
			void setInList(bool inList);
			void setContactInList(bool inList);
			bool hasResource(const QString &resource);
			void addResource(const QString &resource);
			void removeResource(const QString &resource);
			QList<JContactResource *> resources();
			JContactResource *resource(const QString &key);
			virtual ChatUnitList lowerUnits();
			QString avatar() const;
			QString avatarHash() const;
			void setAvatar(const QString &hex);
//			InfoRequest *infoRequest() const;
		protected:
			void fillMaxResource();
			virtual bool event(QEvent *event);
		private:
			friend class JRoster;
			QScopedPointer<JContactPrivate> d_ptr;
	};
}

#endif // JCONTACT_H
