/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Tretyakov Roman <roman@trett.ru>
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

#include "autopaster.h"
#include <qutim/settingslayer.h>
#include "pastersettings.h"

using namespace qutim_sdk_0_3;

void AutoPaster::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Autopaster"),
			QT_TRANSLATE_NOOP("Plugin", "Paste your code to hosting"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("trett"));
	addAuthor(QLatin1String("euroelessar"));
}

void AutoPaster::loadSettings()
{
	m_handler->readSettings();
}

bool AutoPaster::load()
{
	m_handler = new AutoPasterHandler();
	qutim_sdk_0_3::MessageHandler::registerHandler(m_handler,
												   QLatin1String("AutoPaster"),
												   qutim_sdk_0_3::MessageHandler::NormalPriortity,
												   qutim_sdk_0_3::MessageHandler::SenderPriority + 0x2000);

	m_settingsItem = new GeneralSettingsItem<AutoPasterSettings>(
						 Settings::Plugin,	QIcon(),
						 QT_TRANSLATE_NOOP("Plugin", "AutoPaster"));
	Settings::registerItem(m_settingsItem);

	m_settingsItem->connect(SIGNAL(saved()), this, SLOT(loadSettings()));
	return true;
}

bool AutoPaster::unload()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
	m_settingsItem = 0;

	delete m_handler;
	m_handler = 0;

	return true;
}


QUTIM_EXPORT_PLUGIN(AutoPaster)
