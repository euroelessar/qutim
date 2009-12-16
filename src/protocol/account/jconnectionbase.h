#ifndef JCONNECTIONBASE_H
#define JCONNECTIONBASE_H

#include <gloox/connectionbase.h>
#include <gloox/connectiondatahandler.h>
#include "../jprotocol.h"

namespace Jabber
{
	using namespace gloox;

	struct JConnectionBasePrivate;

	class JConnectionBase : public QObject, public ConnectionBase
	{
		Q_OBJECT
		public:
			JConnectionBase(ConnectionDataHandler *cdh);
			virtual ~JConnectionBase();

			virtual ConnectionError connect();
			virtual ConnectionError recv(int timeout = -1);
			virtual bool send(const std::string &data);
			virtual ConnectionError receive();
			virtual void disconnect();
			virtual void cleanup();
			ConnectionState state() const;
			void registerConnectionDataHandler(ConnectionDataHandler *cdh);
			void setServer(const std::string &server, int port = -1);
			const std::string &server() const;
			int port() const;
			virtual int localPort() const;
			virtual const std::string localInterface() const;
			virtual void getStatistics(long int &totalIn, long int &totalOut);
			virtual ConnectionBase *newInstance() const;
		protected:
			ConnectionDataHandler *m_handler;
			ConnectionState m_state;
			std::string m_server;
			int m_port;
		private:
			JConnectionBasePrivate *p;
	};
}

#endif // JCONNECTIONBASE_H
