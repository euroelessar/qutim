/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ivan Vizir <define-true-false@yandex.com>
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

#include "links-openner.h"
#include "cmd-server.h"
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

