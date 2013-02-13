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

void Autopaster::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Autopaster"),
			QT_TRANSLATE_NOOP("Plugin", "Paste your code to hosting"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("trett"));
}

bool Autopaster::load()
{
	m_settingsItem = new GeneralSettingsItem<PasterSettings>(
						 Settings::Plugin,	QIcon(),
						 QT_TRANSLATE_NOOP("Plugin", "AutoPaster"));
	Settings::registerItem(m_settingsItem);

	m_handler = new Handler();
	qutim_sdk_0_3::MessageHandler::registerHandler(m_handler,
												   QLatin1String("Autopaster"),
												   qutim_sdk_0_3::MessageHandler::NormalPriortity,
												   qutim_sdk_0_3::MessageHandler::SenderPriority);

	m_settingsItem->connect(SIGNAL(saved()), m_handler, SLOT(readSettings()));
	return true;
}

bool Autopaster::unload()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;

	delete m_handler;
	m_handler = 0;
	return true;
}


QUTIM_EXPORT_PLUGIN(Autopaster)
