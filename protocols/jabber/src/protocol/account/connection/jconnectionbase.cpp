/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
//#include "jconnectionbase.h"
//#include <qutim/debug.h>
//#include <gloox/clientbase.h>
//#include <qutim/domaininfo.h>

//namespace Jabber
//{
//using namespace gloox;
//using namespace qutim_sdk_0_3;

//JConnectionBase::JConnectionBase(ConnectionDataHandler *cdh) : ConnectionBase(cdh)
//{
//	m_proxy = 0;
//}

//JConnectionBase::~JConnectionBase()
//{
//}

//ConnectionError JConnectionBase::receive()
//{
//	return m_connectionError;
//}

//void JConnectionBase::getStatistics(long int &totalIn, long int &totalOut)
//{
//	totalIn = m_totalBytesIn;
//	totalOut = m_totalBytesOut;
//}

//void JConnectionBase::setUseDns(bool useDns)
//{
//	m_useDns = useDns;
//}

//void JConnectionBase::resolveHost()
//{
//	if (m_useDns) { //FIX IT
//		DomainInfo *info = new DomainInfo(this);
//		QObject::connect(info,SIGNAL(resultReady()),SLOT(hostResolved()));
//		info->lookupSrvRecord(QLatin1String("xmpp-client"),
//							  QLatin1String("tcp"),
//							  QString::fromStdString(m_server));
//		qutim_sdk_0_3::qDebug() << "Looking srv records for:" << QString::fromStdString(m_server);
//	}
//}

//void JConnectionBase::connected()
//{
//	m_connectionError = ConnNoError;
//	m_handler->handleConnect(this);
//}

//void JConnectionBase::disconnected()
//{
//	m_handler->handleDisconnect(this, m_connectionError);
//	cleanup();
//}

//void JConnectionBase::hostFound()
//{
//}

//void JConnectionBase::hostResolved()
//{
//	DomainInfo *info = qobject_cast<DomainInfo*>(sender());
//	Q_ASSERT(info);
//	QList<DomainInfo::Record> records = info->resultRecords();
//	QList<DomainInfo::Record>::const_iterator it = records.constBegin();
//	for(;it!=records.constEnd();it++) {
//		QString host = it->name;
//		int port = it->port;
//		qutim_sdk_0_3::qDebug() << "hosts" << host << port;
//		QString hostr = host;
//		hostr.remove(QRegExp("((\\w|-)+\\.)*(\\w|-)+"));
//		if (hostr.isEmpty() || !QHostAddress( host ).isNull()) {
//			m_server = host.toStdString();
//			m_port = port;
//			qutim_sdk_0_3::qDebug() << "resolved host" << host << port;
//			break;
//		}
//	}
//	hostFound();
//}

//void JConnectionBase::error(QAbstractSocket::SocketError error)
//{
//	if (m_connectionError == ConnUserDisconnected)
//		return;

//	switch (error) {
//	case QAbstractSocket::ConnectionRefusedError:
//		m_connectionError = ConnConnectionRefused;
//		break;
//	case QAbstractSocket::RemoteHostClosedError:
//		m_connectionError = ConnUserDisconnected;
//		break;
//	case QAbstractSocket::HostNotFoundError:
//	case QAbstractSocket::ProxyNotFoundError:
//		m_connectionError = ConnDnsError;
//		break;
//	case QAbstractSocket::ProxyAuthenticationRequiredError:
//		m_connectionError = ConnProxyAuthFailed;
//		break;
//	case QAbstractSocket::SslHandshakeFailedError:
//		m_connectionError = ConnTlsFailed;
//		break;
//	default:
//		m_connectionError = ConnIoError;
//		break;
//	}

//	qDebug() << Q_FUNC_INFO << error;
//}

//void JConnectionBase::stateChanged(QAbstractSocket::SocketState state)
//{
//	switch (state) {
//	case QAbstractSocket::HostLookupState:
//	case QAbstractSocket::ConnectingState:
//		m_state = StateConnecting;
//		break;
//	case QAbstractSocket::ConnectedState:
//		if (m_state != StateConnected) {
//			m_state = StateConnected;
//			connected();
//		}
//		break;
//	case QAbstractSocket::UnconnectedState:
//	case QAbstractSocket::ClosingState:
//	default:
//		if (m_state != StateDisconnected) {
//			m_state = StateDisconnected;
//			disconnected();
//		}
//		break;
//	}

//	qDebug() << Q_FUNC_INFO << state;
//}
//}

