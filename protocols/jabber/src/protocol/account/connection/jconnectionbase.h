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
