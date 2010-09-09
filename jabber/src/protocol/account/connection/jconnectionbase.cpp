#include "jconnectionbase.h"
#include <qutim/debug.h>
#include <gloox/clientbase.h>
#include <qutim/domaininfo.h>

namespace Jabber
{
	using namespace qutim_sdk_0_3;
	
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
			DomainInfo *info = new DomainInfo(this);
			QObject::connect(info,SIGNAL(resultReady()),SLOT(hostResolved()));
			info->lookupSrvRecord(QLatin1String("xmpp-client"),
								  QLatin1String("tcp"),
								  QString::fromStdString(m_server));
			qutim_sdk_0_3::debug() << "Looking srv records for:" << QString::fromStdString(m_server);
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

	void JConnectionBase::hostResolved()
	{
		DomainInfo *info = qobject_cast<DomainInfo*>(sender());
		Q_ASSERT(info);
		QList<DomainInfo::Record> records = info->resultRecords();
		QList<DomainInfo::Record>::const_iterator it = records.constBegin();
		for(;it!=records.constEnd();it++) {
			QString host = it->name;
			int port = it->port;
			qutim_sdk_0_3::debug() << "hosts" << host << port;
			QString hostr = host;
			hostr.remove(QRegExp("((\\w|-)+\\.)*(\\w|-)+"));
			if (hostr.isEmpty() || !QHostAddress( host ).isNull()) {
				m_server = host.toStdString();
				m_port = port;
				qutim_sdk_0_3::debug() << "resolved host" << host << port;				
				break;
			}
		}		
		hostFound();
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
