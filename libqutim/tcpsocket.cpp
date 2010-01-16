/****************************************************************************
 *  tcpsocket.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
	}

	NetworkProxy TcpSocket::proxy() const
	{
		return NetworkProxy();
	}

	void TcpSocket::connectToHostImplementation(const QString &hostName, quint16 port, OpenMode mode)
	{
	}

	void TcpSocket::disconnectFromHostImplementation()
	{
	}
}
