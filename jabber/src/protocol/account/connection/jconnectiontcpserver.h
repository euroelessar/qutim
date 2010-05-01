#ifndef JCONNECTIONTCPSERVER_H
#define JCONNECTIONTCPSERVER_H

#include <gloox/gloox.h>
#include <gloox/connectionbase.h>
#include <gloox/connectionhandler.h>
#include <QTcpServer>

namespace Jabber
{
class JConnectionTcpServer : public QObject, public gloox::ConnectionBase
{
	Q_OBJECT
public:
	JConnectionTcpServer(gloox::ConnectionHandler *handler);
	virtual ~JConnectionTcpServer();
	virtual gloox::ConnectionError recv(int timeout = -1);
	virtual gloox::ConnectionError connect();
	virtual gloox::ConnectionBase *newInstance() const;
	virtual bool send(const std::string &);
	virtual gloox::ConnectionError receive();
	virtual void disconnect();
	virtual void getStatistics(long int &, long int &);
	virtual int localPort() const;
	virtual const std::string localInterface() const;
public slots:
	void newConnection();
private:
	QTcpServer *m_tcp_server;
	gloox::ConnectionHandler *m_connection_handler;
};
}

#endif // JCONNECTIONTCPSERVER_H
