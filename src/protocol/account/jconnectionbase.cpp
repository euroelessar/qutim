#include "jconnectionbase.h"
#include <qutim/debug.h>

namespace Jabber
{
	struct JConnectionBasePrivate
	{
		QTcpSocket *socket;
		QNetworkProxy *proxy;
		bool useDns;
		ConnectionError connectionError;
		qint64 totalBytesIn;
		qint64 totalBytesOut;
	};

	JConnectionBase::JConnectionBase(ConnectionDataHandler *cdh) : p(new JConnectionBasePrivate), ConnectionBase(cdh)
	{
		p->socket = 0;
		p->proxy = 0;
		createSocket();
	}

	JConnectionBase::~JConnectionBase()
	{
		deleteSocket();
	}

	ConnectionError JConnectionBase::connect()
	{
		if (!m_handler)
			return p->connectionError = ConnNotConnected;

		if (p->socket && (p->socket->state() == QAbstractSocket::ConnectingState
				|| p->socket->state() == QAbstractSocket::ConnectedState))
			return p->connectionError = ConnNoError;

		startConnection();
		return p->connectionError;
	}

	ConnectionError JConnectionBase::recv(int timeout)
	{
		read();
		return p->connectionError;
	}

	bool JConnectionBase::send(const std::string &data)
	{
		if (!p->socket)
			return false;

		int bw = p->socket->write(data.c_str());
		if (bw == -1)
			return false;

		p->totalBytesOut += bw;
		p->socket->flush();

		//debug() << Q_FUNC_INFO << QString::fromStdString(data);

		return true;
	}

	ConnectionError JConnectionBase::receive()
	{
		return p->connectionError;
	}

	void JConnectionBase::disconnect()
	{
		p->connectionError = ConnUserDisconnected;
		if (p->socket)
			p->socket->disconnectFromHost();
	}

	void JConnectionBase::cleanup()
	{
		createSocket();
	}

	int JConnectionBase::localPort() const
	{
		return p->socket ? p->socket->localPort() : -1;
	}

	const std::string JConnectionBase::localInterface() const
	{
		return p->socket ? p->socket->localAddress().toString().toStdString() : EmptyString;
	}

	void JConnectionBase::getStatistics(long int &totalIn, long int &totalOut)
	{
		totalIn = p->totalBytesIn;
		totalOut = p->totalBytesOut;
	}

	ConnectionBase *JConnectionBase::newInstance() const
	{
		return new JConnectionBase(m_handler);
	}

	void JConnectionBase::setProxy(QNetworkProxy &proxy)
	{
		p->proxy = &proxy;
		p->socket->setProxy(proxy);
	}

	void JConnectionBase::setUseDns(bool useDns)
	{
		p->useDns = useDns;
	}

	void JConnectionBase::createSocket()
	{
		if (p->socket)
			deleteSocket();

		p->socket = new QTcpSocket();
		p->connectionError = ConnNotConnected;
		p->totalBytesIn = 0;
		p->totalBytesOut = 0;
		if (p->proxy)
			p->socket->setProxy(*p->proxy);

		QObject::connect(p->socket, SIGNAL(connected()), SLOT(connected()));
		QObject::connect(p->socket, SIGNAL(disconnected()), SLOT(disconnected()));
		QObject::connect(p->socket, SIGNAL(hostFound()), SLOT(hostFound()));
		QObject::connect(p->socket, SIGNAL(readyRead()), SLOT(read()));
		QObject::connect(p->socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error(QAbstractSocket::SocketError)));
		QObject::connect(p->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), SLOT(stateChanged(QAbstractSocket::SocketState)));
	}

	void JConnectionBase::deleteSocket()
	{
		if (p->socket)
			p->socket->deleteLater();
	}

	void JConnectionBase::startConnection()
	{
		if (p->useDns) { //FIX IT
			//debug() << "Server:" << QString::fromStdString(m_server) << "port:" << m_port;
			DNS::HostMap hosts = DNS::resolve(m_server, LogSink());
			DNS::HostMap::iterator h = hosts.begin();
			for(;h!=hosts.end();h++)
			{
				QString host = QString::fromStdString(h->first);
				QString hostr = host;
				hostr.remove(QRegExp("(\\w+\\.)+\\w+"));
				if(hostr.isEmpty() || !QHostAddress( host ).isNull()) {
					//debug() << "DNS: Server:" << host << "port:" << h->second;
					m_server = host.toStdString();
					m_port = h->second;
					break;
				}
			}
		}

		p->socket->connectToHost(QString::fromStdString(m_server), m_port);
	}

	void JConnectionBase::connected()
	{
		//debug() << Q_FUNC_INFO;
		p->connectionError = ConnNoError;
		m_handler->handleConnect(this);
	}

	void JConnectionBase::disconnected()
	{
		//debug() << Q_FUNC_INFO;
		m_handler->handleDisconnect(this, p->connectionError);
		cleanup();
	}

	void JConnectionBase::hostFound()
	{
		//debug() << Q_FUNC_INFO;
	}

	void JConnectionBase::read()
	{
		if (!p->socket)
			return;

		qint64 ba = p->socket->bytesAvailable();
		if (ba <= 0)
			return;

		QByteArray data = p->socket->read(ba);
		p->totalBytesIn += data.size();
		m_handler->handleReceivedData(this, data.data());

		//debug() << Q_FUNC_INFO << QString(data);
	}

	void JConnectionBase::error(QAbstractSocket::SocketError error)
	{
		//debug() << Q_FUNC_INFO << error;
		if (p->connectionError == ConnUserDisconnected)
			return;

		switch (error) {
			case QAbstractSocket::ConnectionRefusedError:
				p->connectionError = ConnConnectionRefused;
				break;
			case QAbstractSocket::RemoteHostClosedError:
				p->connectionError = ConnUserDisconnected;
				break;
			case QAbstractSocket::HostNotFoundError:
			case QAbstractSocket::ProxyNotFoundError:
				p->connectionError = ConnDnsError;
				break;
			case QAbstractSocket::ProxyAuthenticationRequiredError:
				p->connectionError = ConnProxyAuthFailed;
				break;
			case QAbstractSocket::SslHandshakeFailedError:
				p->connectionError = ConnTlsFailed;
				break;
			default:
				p->connectionError = ConnIoError;
				break;
		}
	}

	void JConnectionBase::stateChanged(QAbstractSocket::SocketState state)
	{
		//debug() << Q_FUNC_INFO << state;
		switch (state) {
			case QAbstractSocket::HostLookupState:
			case QAbstractSocket::ConnectingState:
				m_state = StateConnecting;
				break;
			case QAbstractSocket::ConnectedState:
				m_state = StateConnected;
				break;
			case QAbstractSocket::UnconnectedState:
			case QAbstractSocket::ClosingState:
			default:
				m_state = StateDisconnected;
				break;
		}
	}

}
