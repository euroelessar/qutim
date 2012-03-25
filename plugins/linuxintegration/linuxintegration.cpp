/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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
** along with this program. If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "linuxintegration.h"
#include <qutim/utils.h>
#include <QAbstractSocket>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace qutim_sdk_0_3;

LinuxIntegration::LinuxIntegration()
{
}

void LinuxIntegration::init()
{
}

bool LinuxIntegration::isAvailable() const
{
	return true;
}

int LinuxIntegration::priority()
{
	return BaseSystem;
}

QVariant LinuxIntegration::doGetValue(SystemIntegration::Attribute attr, const QVariant &data) const
{
	return QVariant();
}

QVariant LinuxIntegration::doProcess(SystemIntegration::Operation act, const QVariant &data) const
{
	if (act == KeepAliveSocket) {
		QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(data.value<QObject*>());
		if (socket) {
			connect(socket, SIGNAL(connected()), SLOT(onSocketConnected()), Qt::UniqueConnection);
			keepAliveSocket(socket->socketDescriptor());
		}
	}
	return QVariant();
}

bool LinuxIntegration::canHandle(SystemIntegration::Attribute attribute) const
{
	return false;
}

bool LinuxIntegration::canHandle(SystemIntegration::Operation operation) const
{
	return operation == KeepAliveSocket;
}

void LinuxIntegration::onSocketConnected()
{
	QAbstractSocket *socket = sender_cast<QAbstractSocket*>(sender());
	keepAliveSocket(socket->socketDescriptor());
}

void LinuxIntegration::keepAliveSocket(int fd)
{
	if (fd == -1)
		return;
	int enableKeepAlive = 1;
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive));

	int maxIdle = 15; // seconds
	setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));

	int count = 3;  // send up to 3 keepalive packets out, then disconnect if no response
	setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));

	int interval = 2;   // send a keepalive packet out every 2 seconds (after the idle period)
	setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
}
