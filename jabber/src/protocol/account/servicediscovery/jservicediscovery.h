#ifndef JSERVICEDISCOVERY_H
#define JSERVICEDISCOVERY_H

#include <QObject>
#include <gloox/discohandler.h>
#include <gloox/error.h>

namespace Jabber
{
	using namespace gloox;

	struct JServicePrivate;
	class JAccount;
	class JDiscoItem;
	class JServiceReceiver;

	class JServiceDiscovery : public QObject, public DiscoHandler
	{
		Q_OBJECT
		public:
			JServiceDiscovery(JAccount *account);
			~JServiceDiscovery();
			int getInfo(JServiceReceiver *receiver, const JDiscoItem &di);
			int getItems(JServiceReceiver *receiver, const JDiscoItem &di);
			void handleDiscoInfo(const JID &from, const Disco::Info &info, int context);
			void handleDiscoItems(const JID &from, const Disco::Items &items, int context);
			void handleDiscoError(const JID &from, const Error *error, int context);
			bool handleDiscoSet(const IQ &iq);
		protected:
			void addDiscoIdentity(JDiscoItem &di, Disco::Identity *identity);
			void setActions(JDiscoItem &di);
		private:
			QScopedPointer<JServicePrivate> p;
	};
}

#endif //JSERVICEDISCOVERY_H
