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
#ifndef JCONNECTIONTCPBASE_H
#define JCONNECTIONTCPBASE_H

//#include "jconnectionbase.h"

//#include <QtNetwork/QTcpSocket>

//namespace Jabber
//{

//	struct JConnectionTCPBasePrivate;

//	class JConnectionTCPBase : public JConnectionBase
//	{
//		Q_OBJECT
//	public:
//		JConnectionTCPBase(gloox::ConnectionDataHandler *cdh);
//		JConnectionTCPBase(QTcpSocket *socket);
//		virtual ~JConnectionTCPBase();

//		virtual gloox::ConnectionError connect();
//		virtual gloox::ConnectionError recv(int timeout = -1);
//		virtual bool send(const std::string &data);
//		virtual void disconnect();
//		virtual void cleanup();
//		virtual int localPort() const;
//		virtual const std::string localInterface() const;
//		virtual gloox::ConnectionBase *newInstance() const;
//		virtual void setProxy(QNetworkProxy &proxy);
//	public slots:
//		void read();
//	protected slots:
//		virtual void hostFound();
//	private:
//		void createSocket();
//		void deleteSocket();
//		void startConnection();
//		JConnectionTCPBasePrivate *p;
//	};
//}

#endif // JCONNECTIONTCPBASE_H

