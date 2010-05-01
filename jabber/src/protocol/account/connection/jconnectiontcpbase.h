#ifndef JCONNECTIONTCPBASE_H
#define JCONNECTIONTCPBASE_H

#include "jconnectionbase.h"

#include <QtNetwork/QTcpSocket>

namespace Jabber
{
	using namespace gloox;

	struct JConnectionTCPBasePrivate;

	class JConnectionTCPBase : public JConnectionBase
	{
		Q_OBJECT
	public:
		JConnectionTCPBase(ConnectionDataHandler *cdh);
		JConnectionTCPBase(QTcpSocket *socket);
		virtual ~JConnectionTCPBase();

		virtual ConnectionError connect();
		virtual ConnectionError recv(int timeout = -1);
		virtual bool send(const std::string &data);
		virtual void disconnect();
		virtual void cleanup();
		virtual int localPort() const;
		virtual const std::string localInterface() const;
		virtual ConnectionBase *newInstance() const;
		virtual void setProxy(QNetworkProxy &proxy);
	public slots:
		void read();
	private:
		void createSocket();
		void deleteSocket();
		void startConnection();
		JConnectionTCPBasePrivate *p;
	};
}

#endif // JCONNECTIONTCPBASE_H
