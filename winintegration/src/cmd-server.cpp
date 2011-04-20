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
