/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "highlighter.h"
#include "highlightersettings.h"
#include <qutim/config.h>
#include <qutim/settingslayer.h>
#include "nickhandler.h"

namespace Highlighter
{

using namespace qutim_sdk_0_3;

void HighlighterPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Highlighter"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin that add support of highlighting, when your nick has been mentioned."),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QT_TRANSLATE_NOOP("Author","Nicolay Izoderov"),
			  QT_TRANSLATE_NOOP("Task","Developer"),
			  QLatin1String("nico-izo@ya.ru"));
}

bool HighlighterPlugin::load()
{
	m_settingsItem = new GeneralSettingsItem<HighlighterSettings>(
				Settings::Plugin,	QIcon(),
				QT_TRANSLATE_NOOP("Plugin", "Highlighter"));
	Settings::registerItem(m_settingsItem);

	if (!m_handler)
		m_handler = new NickHandler;
	qutim_sdk_0_3::MessageHandler::registerHandler(m_handler.data(),
												   qutim_sdk_0_3::MessageHandler::HighPriority,
												   qutim_sdk_0_3::MessageHandler::HighPriority);
	m_settingsItem->connect(SIGNAL(saved()), m_handler.data(), SLOT(loadSettings()));
	return true;
}

bool HighlighterPlugin::unload()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;

	if (m_handler) {
		m_handler.data()->unregisterHandler(m_handler.data());
		m_handler.data()->deleteLater();
		return true;
	}
	return false;
}

}

QUTIM_EXPORT_PLUGIN(Highlighter::HighlighterPlugin)

