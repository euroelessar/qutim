/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
//#include "jconnectiontcpserver.h"
//#include "jconnectiontcpbase.h"

//using namespace gloox;

//namespace Jabber
//{
//JConnectionTcpServer::JConnectionTcpServer(ConnectionHandler* connection_handler)
//	: QObject(0), ConnectionBase(0), m_tcp_server(0), m_connection_handler(connection_handler)
//{
//}

//JConnectionTcpServer::~JConnectionTcpServer()
//{
//	if (m_tcp_server)
//		delete m_tcp_server;
//}

//ConnectionError JConnectionTcpServer::recv(int )
//{
//	if (m_tcp_server->isListening())
//		return ConnNoError;
//	else
//		return ConnNotConnected;
//}

//ConnectionError JConnectionTcpServer::connect()
//{
//	if (!m_tcp_server) {
//		m_tcp_server = new QTcpServer();
//		QObject::connect( m_tcp_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
//	}

//	if(m_tcp_server->isListening())
//		return ConnNoError;

//	bool result;
//	if (m_server.empty())
//		result = m_tcp_server->listen(QHostAddress::Any, m_port);
//	else
//		result = m_tcp_server->listen(QHostAddress(QString::fromStdString(m_server)), m_port);
//	qDebug() << "server connect" << QString::fromStdString(m_server) << m_port << result << m_tcp_server->errorString();
//	if(result)
//		return ConnNoError;
//	else
//		return ConnIoError;
//}

//ConnectionBase* JConnectionTcpServer::newInstance() const
//{
//	JConnectionTcpServer *server = new JConnectionTcpServer(m_connection_handler);
//	server->setServer(m_server, m_port);
//	return server;
//}

//void JConnectionTcpServer::newConnection()
//{
//	QTcpSocket *socket = m_tcp_server->nextPendingConnection();
//	JConnectionTCPBase *conn = new JConnectionTCPBase(socket);
//	conn->setServer(socket->peerAddress().toString().toStdString(), socket->peerPort());
//	m_connection_handler->handleIncomingConnection(this, conn);
//	qDebug() << "new connection" << socket->bytesAvailable() << socket->state();
//	conn->read();
//}

//bool JConnectionTcpServer::send(const std::string&)
//{
//	return false;
//}

//ConnectionError JConnectionTcpServer::receive()
//{
//	return recv();
//}
//void JConnectionTcpServer::disconnect()
//{
//	m_tcp_server->close();
//}

//void JConnectionTcpServer::getStatistics(long int &in, long int &out)
//{
//	in = 0;
//	out = 0;
//}

//int JConnectionTcpServer::localPort() const
//{
//	return m_tcp_server ? m_tcp_server->serverPort() : -1;
//}

//const std::string JConnectionTcpServer::localInterface() const
//{
//	return m_tcp_server ? m_tcp_server->serverAddress().toString().toStdString() : EmptyString;
//}
//}

