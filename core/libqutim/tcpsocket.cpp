/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "tcpsocket.h"

namespace qutim_sdk_0_3
{
	class NetworkProxy
	{
	};

	TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), p(0)
	{
	}

	TcpSocket::~TcpSocket()
	{
	//	delete p;
	}

	void TcpSocket::setProxy(const NetworkProxy &networkProxy)
	{
		Q_UNUSED(networkProxy);
	}

	NetworkProxy TcpSocket::proxy() const
	{
		return NetworkProxy();
	}

	void TcpSocket::connectToHostImplementation(const QString &hostName, quint16 port, OpenMode mode)
	{
		Q_UNUSED(hostName);
		Q_UNUSED(port);
		Q_UNUSED(mode);
	}

	void TcpSocket::disconnectFromHostImplementation()
	{
	}
}

