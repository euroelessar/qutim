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

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "libqutim_global.h"
#include <QTcpSocket>

namespace qutim_sdk_0_3
{
	class NetworkProxy;
	class TcpSocketPrivate;

	class LIBQUTIM_EXPORT TcpSocket : public QTcpSocket
	{
		Q_OBJECT
	public:
		explicit TcpSocket(QObject *parent = 0);
		virtual ~TcpSocket();
		void setProxy(const NetworkProxy &networkProxy);
		NetworkProxy proxy() const;

	protected Q_SLOTS:
		void connectToHostImplementation(const QString &hostName, quint16 port, OpenMode mode = ReadWrite);
		void disconnectFromHostImplementation();

	protected:
		TcpSocketPrivate *p;
	};
}

#endif // TCPSOCKET_H

