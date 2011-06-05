/****************************************************************************
 * ulservice.cpp
 *  Copyright © 2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv2 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include "ulservice.h"
#include "unitydock.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/chatsession.h>
#include <QDateTime>
#include <QApplication>

UnityLauncherService::UnityLauncherService(QObject *p) :
	QObject(p),
	sessionCount(0),
	dock(new UnityDock(this))
{
	qApp->setQuitOnLastWindowClosed(false);
	connect(
			qutim_sdk_0_3::ChatLayer::instance(),
			SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*))
	       );
	connect(
			qutim_sdk_0_3::ChatLayer::instance(),
			SIGNAL(alertStatusChanged(bool)),
			SLOT(setAlert(bool))
	       );
	foreach (qutim_sdk_0_3::ChatSession *session, qutim_sdk_0_3::ChatLayer::instance()->sessions())
		onSessionCreated(session);
	qutim_sdk_0_3::ServicePointer<qutim_sdk_0_3::MenuController> clist("ContactList");
	dock->setMenu(clist->menu(false));
}

UnityLauncherService::~UnityLauncherService()
{
}

void UnityLauncherService::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	qutim_sdk_0_3::debug() << "[UnityLauncher] onSessionCreated";
	dock->setCount(++sessionCount);
	connect(session, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList&)), SLOT(sendAlert()));
}

void UnityLauncherService::onSessionDestroyed(QObject *)
{
	qutim_sdk_0_3::debug() << "[UnityLauncher] onSessionDestroyed";
	dock->setCount(--sessionCount);
}

void UnityLauncherService::sendAlert()
{
	qutim_sdk_0_3::ChatLayer::instance()->alert(300);
}

void UnityLauncherService::setAlert(bool on)
{
	dock->setAlert(on);
}
