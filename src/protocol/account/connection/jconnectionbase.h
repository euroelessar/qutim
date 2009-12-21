#ifndef JCONNECTIONBASE_H
#define JCONNECTIONBASE_H

#include <gloox/connectionbase.h>
#include <gloox/connectiondatahandler.h>
#include <gloox/dns.h>

#include <QtCore/QRegExp>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkProxy>

namespace Jabber
{
	using namespace gloox;

	class JConnectionBase : public QObject, public ConnectionBase
	{
		Q_OBJECT
		public:
			JConnectionBase(ConnectionDataHandler *cdh);
			virtual ~JConnectionBase();

			virtual ConnectionError receive();
			virtual void getStatistics(long int &totalIn, long int &totalOut);
			virtual void setProxy(QNetworkProxy &proxy) = 0;
			void resolveHost();
			void setUseDns(bool useDns);
		protected slots:
			virtual void connected();
			virtual void disconnected();
			virtual void hostFound();
			virtual void error(QAbstractSocket::SocketError error);
			virtual void stateChanged(QAbstractSocket::SocketState state);
		protected:
			QNetworkProxy *m_proxy;
			qint64 m_totalBytesIn;
			qint64 m_totalBytesOut;
			ConnectionError m_connectionError;
			bool m_useDns;
		private:

	};
}

#endif // JCONNECTIONBASE_H
