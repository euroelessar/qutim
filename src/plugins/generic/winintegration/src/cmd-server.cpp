/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ivan Vizir <define-true-false@yandex.com>
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

#include "cmd-server.h"
#include <QTcpSocket>
#include <qutim/debug.h>

const int connectPort = 35134;

CmdServer::CmdServer()
{
	connect(&listenner_, SIGNAL(newConnection()), SLOT(onNewConnection()));
	listenner_.listen(QHostAddress::LocalHost, connectPort);
}

void CmdServer::onNewConnection()
{
	QTcpSocket *sk = listenner_.nextPendingConnection();
	sk->waitForDisconnected(50);
	QString cmd = sk->readAll();
	QString handlerStr  = cmd.section(" ", 0, 0);
	QObject *handlerObj = handlers_[handlerStr];
	if (handlerObj)
		QMetaObject::invokeMethod(handlerObj, "commandReceived", Q_ARG(QString, cmd.section(" ", 1)));
}

void CmdServer::registerHandler(const QString &prefix, QObject *receiver)
{
	handlers_[prefix] = receiver;
	connect(receiver, SIGNAL(destroyed(QObject*)), SLOT(onRcvDestroyed(QObject*)));
}

void CmdServer::onRcvDestroyed(QObject *obj)
{
	handlers_.remove(handlers_.key(obj));
}

CmdServer *CmdServer::instance()
{
	static CmdServer inst; return &inst;
}

