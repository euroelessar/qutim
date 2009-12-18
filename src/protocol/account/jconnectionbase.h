#ifndef JCONNECTIONBASE_H
#define JCONNECTIONBASE_H

#include <gloox/connectionbase.h>
#include <gloox/connectiondatahandler.h>
#include <gloox/dns.h>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
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
			virtual int localPort() const;
			virtual const std::string localInterface() const;
			virtual void getStatistics(long int &totalIn, long int &totalOut);
			virtual ConnectionBase *newInstance() const;

			void setProxy(QNetworkProxy &proxy);
			void setUseDns(bool useDns);
		private slots:
			void connected();
			void disconnected();
			void hostFound();
			void read();
			void error(QAbstractSocket::SocketError error);
			void stateChanged(QAbstractSocket::SocketState state);
		private:
			void createSocket();
			void deleteSocket();
			void startConnection();
			JConnectionBasePrivate *p;
	};
}

#endif // JCONNECTIONBASE_H
