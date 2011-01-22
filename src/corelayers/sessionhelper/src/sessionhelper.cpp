/*
  sessionhelper.cpp

  (c) 2011 by Aleksey Sidorov <sauron@citadelspb.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "sessionhelper.h"
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/protocol.h>

namespace SessionHelper
{

void SessionHelper::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Session helper"),
			QT_TRANSLATE_NOOP("Plugin", "Activates the session when certain events"),
			PLUGIN_VERSION(0, 1, 0, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task","Developer"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
}

bool SessionHelper::load()
{
	ChatLayer *layer = ChatLayer::instance();
	connect(layer,SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(sessionCreated(qutim_sdk_0_3::ChatSession*))
			);
	return true;
}

bool SessionHelper::unload()
{
	return true;
}

void SessionHelper::sessionCreated(qutim_sdk_0_3::ChatSession* session)
{
	session->activate();
}

} //namespace SessionHelper

QUTIM_EXPORT_PLUGIN(SessionHelper::SessionHelper);
