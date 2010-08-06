#include "jconnectionbase.h"
#include <qutim/debug.h>

namespace Jabber
{
	JConnectionBase::JConnectionBase(ConnectionDataHandler *cdh) : ConnectionBase(cdh)
	{
		m_proxy = 0;
	}

	JConnectionBase::~JConnectionBase()
	{
	}

	ConnectionError JConnectionBase::receive()
	{
		return m_connectionError;
	}

	void JConnectionBase::getStatistics(long int &totalIn, long int &totalOut)
	{
		totalIn = m_totalBytesIn;
		totalOut = m_totalBytesOut;
	}

	void JConnectionBase::setUseDns(bool useDns)
	{
		m_useDns = useDns;
	}

	void JConnectionBase::resolveHost()
	{
		if (m_useDns) { //FIX IT
			static LogSink emptyLogSync;
			DNS::HostMap hosts = DNS::resolve(m_server, emptyLogSync);
			qutim_sdk_0_3::debug() << "server" << QString::fromStdString(m_server);
			DNS::HostMap::iterator h = hosts.begin();
			for (; h!=hosts.end(); h++) {				
				QString host = QString::fromStdString(h->first);
				qutim_sdk_0_3::debug() << "hosts" << host << h->second;
				QString hostr = host;
				hostr.remove(QRegExp("((\\w|-)+\\.)*(\\w|-)+"));
				if (hostr.isEmpty() || !QHostAddress( host ).isNull()) {
					m_server = host.toStdString();
					m_port = h->second;
					qutim_sdk_0_3::debug() << "resolved host" << host << h->second;
					break;
				}
			}
		}
	}

	void JConnectionBase::connected()
	{
		m_connectionError = ConnNoError;
		m_handler->handleConnect(this);
	}

	void JConnectionBase::disconnected()
	{
		m_handler->handleDisconnect(this, m_connectionError);
		cleanup();
	}

	void JConnectionBase::hostFound()
	{
	}

	void JConnectionBase::error(QAbstractSocket::SocketError error)
	{
		if (m_connectionError == ConnUserDisconnected)
			return;

		switch (error) {
			case QAbstractSocket::ConnectionRefusedError:
				m_connectionError = ConnConnectionRefused;
				break;
			case QAbstractSocket::RemoteHostClosedError:
				m_connectionError = ConnUserDisconnected;
				break;
			case QAbstractSocket::HostNotFoundError:
			case QAbstractSocket::ProxyNotFoundError:
				m_connectionError = ConnDnsError;
				break;
			case QAbstractSocket::ProxyAuthenticationRequiredError:
				m_connectionError = ConnProxyAuthFailed;
				break;
			case QAbstractSocket::SslHandshakeFailedError:
				m_connectionError = ConnTlsFailed;
				break;
			default:
				m_connectionError = ConnIoError;
				break;
		}

		qDebug() << Q_FUNC_INFO << error;
	}

	void JConnectionBase::stateChanged(QAbstractSocket::SocketState state)
	{
		switch (state) {
			case QAbstractSocket::HostLookupState:
			case QAbstractSocket::ConnectingState:
				m_state = StateConnecting;
				break;
			case QAbstractSocket::ConnectedState:
				if (m_state != StateConnected) {
					m_state = StateConnected;
					connected();
				}
				break;
			case QAbstractSocket::UnconnectedState:
			case QAbstractSocket::ClosingState:
			default:
				if (m_state != StateDisconnected) {
					m_state = StateDisconnected;
					disconnected();
				}
				break;
		}

		qDebug() << Q_FUNC_INFO << state;
	}
}
