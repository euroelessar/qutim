#ifndef JCONTACTRESOURCE_H
#define JCONTACTRESOURCE_H

#include <qutim/chatunit.h>
#include <gloox/presence.h>
#include <QSet>

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	class JAccount;
	class JContact;
	struct JResourcePrivate;

	class JContactResource : public ChatUnit
	{
		Q_PROPERTY(QSet<QString> features READ features WRITE setFeatures)
		Q_OBJECT
		public:
			JContactResource(JContact *contact, const QString &name);
			~JContactResource();
			QString id() const;
			void sendMessage(const qutim_sdk_0_3::Message &message);
			void setPriority(int priority);
			int priority();
			void setStatus(gloox::Presence::PresenceType presence, int priority);
			gloox::Presence::PresenceType status();
			void setChatState(ChatState state);
			QSet<QString> features() const;
			void setFeatures(const QSet<QString> &features);
			bool checkFeature(const QLatin1String &feature) const;
			bool checkFeature(const QString &feature) const;
			bool checkFeature(const std::string &feature) const;
		private:
			QScopedPointer<JResourcePrivate> p;
	};
}

Q_DECLARE_METATYPE(QSet<QString>)

#endif // JCONTACTRESOURCE_H
