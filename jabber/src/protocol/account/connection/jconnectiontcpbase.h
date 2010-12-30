#ifndef JCONNECTIONTCPBASE_H
#define JCONNECTIONTCPBASE_H

#include "jconnectionbase.h"

#include <QtNetwork/QTcpSocket>

namespace Jabber
{

	struct JConnectionTCPBasePrivate;

	class JConnectionTCPBase : public JConnectionBase
	{
		Q_OBJECT
	public:
		JConnectionTCPBase(gloox::ConnectionDataHandler *cdh);
		JConnectionTCPBase(QTcpSocket *socket);
		virtual ~JConnectionTCPBase();

		virtual gloox::ConnectionError connect();
		virtual gloox::ConnectionError recv(int timeout = -1);
		virtual bool send(const std::string &data);
		virtual void disconnect();
		virtual void cleanup();
		virtual int localPort() const;
		virtual const std::string localInterface() const;
		virtual gloox::ConnectionBase *newInstance() const;
		virtual void setProxy(QNetworkProxy &proxy);
	public slots:
		void read();
	protected slots:
		virtual void hostFound();
	private:
		void createSocket();
		void deleteSocket();
		void startConnection();
		JConnectionTCPBasePrivate *p;
	};
}

#endif // JCONNECTIONTCPBASE_H
