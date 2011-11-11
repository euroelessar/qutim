/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef JCONNECTIONBASE_H
#define JCONNECTIONBASE_H

//#include <gloox/connectionbase.h>
//#include <gloox/connectiondatahandler.h>
////#include <gloox/dns.h>
//#include <QtCore/QRegExp>
//#include <QtNetwork/QHostAddress>
//#include <QtNetwork/QNetworkProxy>

//namespace Jabber
//{

//	class JConnectionBase : public QObject, public gloox::ConnectionBase
//	{
//		Q_OBJECT
//		public:
//			JConnectionBase(gloox::ConnectionDataHandler *cdh);
//			virtual ~JConnectionBase();

//			virtual gloox::ConnectionError receive();
//			virtual void getStatistics(long int &totalIn, long int &totalOut);
//			virtual void setProxy(QNetworkProxy &proxy) = 0;
//			void resolveHost();
//			void setUseDns(bool useDns);
//		protected slots:
//			virtual void connected();
//			virtual void disconnected();
//			virtual void hostFound();
//			void hostResolved();
//			virtual void error(QAbstractSocket::SocketError error);
//			virtual void stateChanged(QAbstractSocket::SocketState state);
//		protected:
//			QNetworkProxy *m_proxy;
//			qint64 m_totalBytesIn;
//			qint64 m_totalBytesOut;
//			gloox::ConnectionError m_connectionError;
//			bool m_useDns;
//		private:

//	};
//}

#endif // JCONNECTIONBASE_H

