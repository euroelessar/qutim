#include "links-openner.h"
#include "cmd-server.h"
#include "selectaccdlg.h"
#include <QUrl>
#include <qutim/debug.h>
#include <qutim/chatsession.h>
#include <qutim/chatunit.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/servicemanager.h>

using namespace qutim_sdk_0_3;

LinksOpenner::LinksOpenner(QObject *parent) :
    QObject(parent)
{
	CmdServer::instance()->registerHandler("xmpp-proto", this);
}

void LinksOpenner::commandReceived(QString openedJid)
{
	QObject *handler = ServiceManager::getByName("UriHandler");
	if (handler)
		QMetaObject::invokeMethod(handler, "open", Q_ARG(QString, openedJid));
}
