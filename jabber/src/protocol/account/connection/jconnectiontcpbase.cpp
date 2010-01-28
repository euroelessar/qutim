#include "jconnectiontcpbase.h"

namespace Jabber
{
	struct JConnectionTCPBasePrivate
	{
		QTcpSocket *socket;
	};

	JConnectionTCPBase::JConnectionTCPBase(ConnectionDataHandler *cdh) : p(new JConnectionTCPBasePrivate),
			JConnectionBase(cdh)
	{
		p->socket = 0;
		createSocket();
	}

	JConnectionTCPBase::~JConnectionTCPBase()
	{
		deleteSocket();
	}

	ConnectionError JConnectionTCPBase::connect()
	{
		if (!m_handler)
			return m_connectionError = ConnNotConnected;

		if (p->socket && (p->socket->state() == QAbstractSocket::ConnectingState
				|| p->socket->state() == QAbstractSocket::ConnectedState))
			return m_connectionError = ConnNoError;

		startConnection();
		return m_connectionError;
	}

	ConnectionError JConnectionTCPBase::recv(int timeout)
	{
		read();
		return m_connectionError;
	}

	bool JConnectionTCPBase::send(const std::string &data)
	{
		if (!p->socket)
			return false;

		int bw = p->socket->write(data.c_str(), data.length());
		if (bw == -1)
			return false;

		m_totalBytesOut += bw;
		p->socket->flush();

		return true;
	}

	void JConnectionTCPBase::disconnect()
	{
		m_connectionError = ConnUserDisconnected;
		if (p->socket)
			p->socket->disconnectFromHost();
	}

	void JConnectionTCPBase::cleanup()
	{
		createSocket();
	}

	int JConnectionTCPBase::localPort() const
	{
		return p->socket ? p->socket->localPort() : -1;
	}

	const std::string JConnectionTCPBase::localInterface() const
	{
		return p->socket ? p->socket->localAddress().toString().toStdString() : EmptyString;
	}

	ConnectionBase *JConnectionTCPBase::newInstance() const
	{
		return new JConnectionTCPBase(m_handler);
	}

	void JConnectionTCPBase::createSocket()
	{
		if (p->socket)
			deleteSocket();

		p->socket = new QTcpSocket();
		m_connectionError = ConnNotConnected;
		m_totalBytesIn = 0;
		m_totalBytesOut = 0;
		if (m_proxy)
			p->socket->setProxy(*m_proxy);

		QObject::connect(p->socket, SIGNAL(connected()), SLOT(connected()));
		QObject::connect(p->socket, SIGNAL(disconnected()), SLOT(disconnected()));
		QObject::connect(p->socket, SIGNAL(hostFound()), SLOT(hostFound()));
		QObject::connect(p->socket, SIGNAL(readyRead()), SLOT(read()));
		QObject::connect(p->socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
		QObject::connect(p->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	}

	void JConnectionTCPBase::deleteSocket()
	{
		if (p->socket)
			p->socket->deleteLater();
	}

	void JConnectionTCPBase::startConnection()
	{
		resolveHost();
		p->socket->connectToHost(QString::fromStdString(m_server), m_port);
	}

	void JConnectionTCPBase::read()
	{
		if (!p->socket)
			return;

		qint64 ba = p->socket->bytesAvailable();
		if (ba <= 0)
			return;

		QByteArray data = p->socket->read(ba);
		m_totalBytesIn += data.size();
		m_handler->handleReceivedData(this, std::string(data.data(), data.size()));
	}

	void JConnectionTCPBase::setProxy(QNetworkProxy &proxy)
	{
		m_proxy = &proxy;
		p->socket->setProxy(proxy);
	}
}
