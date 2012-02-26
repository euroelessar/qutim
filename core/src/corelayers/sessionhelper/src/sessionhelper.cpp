/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "sessionhelper.h"
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/conference.h>
#include <QTimer>

namespace SessionHelper
{

void SessionHelper::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Session helper"),
			QT_TRANSLATE_NOOP("Plugin", "Activates the session when certain events"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("sauron"));
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
	//TODO write more flexible 
	if(qobject_cast<Conference*>(session->unit()))
		QTimer::singleShot(0, session, SLOT(activate()));
}

} //namespace SessionHelper

QUTIM_EXPORT_PLUGIN(SessionHelper::SessionHelper);

