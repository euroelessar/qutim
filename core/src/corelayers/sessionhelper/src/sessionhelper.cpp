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
#include "sessionhelpersettings.h"
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/conference.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
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
	addAuthor(QLatin1String("nicoizo"));
}

bool SessionHelper::load()
{
	m_settingsItem = new GeneralSettingsItem<Core::SessionHelperSettings>(Settings::General, Icon("view-choose"),
																		QT_TRANSLATE_NOOP("Settings","Chat"));

	ChatLayer *layer = ChatLayer::instance();
	connect(layer,SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(sessionCreated(qutim_sdk_0_3::ChatSession*))
			);

	m_settingsItem->setOrder(100);
	Settings::registerItem(m_settingsItem);
	m_settingsItem->connect(SIGNAL(saved()), this, SLOT(reloadSettings()));
	reloadSettings();
	return true;
}

bool SessionHelper::unload()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
	return true;
}

void SessionHelper::sessionCreated(qutim_sdk_0_3::ChatSession* session)
{
	if(m_activateMultichat) {
		if(qobject_cast<Conference*>(session->unit()))
			QTimer::singleShot(0, session, SLOT(activate()));
	}
}

void SessionHelper::reloadSettings()
{
	Config config("appearance");
	config.beginGroup("chat/behavior/widget");
	m_activateMultichat = config.value<bool>("activateMultichat", true);
}

} //namespace SessionHelper

QUTIM_EXPORT_PLUGIN(SessionHelper::SessionHelper)

