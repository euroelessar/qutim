#ifndef JCONNECTIONLISTENER_H
#define JCONNECTIONLISTENER_H

#include <gloox/connectionlistener.h>
#include "jcertinfo.h"

namespace Jabber
{
	using namespace gloox;

	class JAccount;
	struct JConnectionListenerPrivate;

	class JConnectionListener : public QObject, public ConnectionListener
	{
		Q_OBJECT
		public:
			JConnectionListener(JAccount *account);
			virtual ~JConnectionListener();

			virtual void onConnect();
			virtual void onDisconnect(ConnectionError error);
			virtual void onResourceBind(const std::string &resource);
			virtual void onResourceBindError(const Error *error);
			virtual void onSessionCreateError(const Error *error);
			virtual bool onTLSConnect(const CertInfo &info);
			virtual void onStreamEvent(StreamEvent event);
		private:
			JConnectionListenerPrivate *p;
	};
}

#endif // JCONNECTIONLISTENER_H
