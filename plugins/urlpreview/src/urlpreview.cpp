/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#include "urlpreview.h"
#include "urlpreviewsettings.h"
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/settingslayer.h>
#include "messagehandler.h"

namespace UrlPreview
{

using namespace qutim_sdk_0_3;

void UrlPreviewPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "UrlPreview"),
			QT_TRANSLATE_NOOP("Plugin", "Preview images directly in the chat window"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("boiler"));
	addAuthor(QLatin1String("sauron"));
	addAuthor(QLatin1String("nicoizo"));
}

bool UrlPreviewPlugin::load()
{
	m_settingsItem = new GeneralSettingsItem<UrlPreviewSettings>(
				Settings::Plugin,	QIcon(),
				QT_TRANSLATE_NOOP("Plugin", "UrlPreview"));
	Settings::registerItem(m_settingsItem);

	if (!m_handler)
		m_handler = new UrlHandler;
	qutim_sdk_0_3::MessageHandler::registerHandler(m_handler.data(),
												   qutim_sdk_0_3::MessageHandler::HighPriority,
												   qutim_sdk_0_3::MessageHandler::HighPriority);
	m_settingsItem->connect(SIGNAL(saved()), m_handler.data(), SLOT(loadSettings()));
	return true;
}

bool UrlPreviewPlugin::unload()
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

QUTIM_EXPORT_PLUGIN(UrlPreview::UrlPreviewPlugin)

