/****************************************************************************
 *  tcpsocket.h
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
