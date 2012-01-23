/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "antispamplugin.h"
#include <qutim/debug.h>
#include <qutim/messagesession.h>
#include "handler.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include "settingswidget.h"

namespace Antispam
{

using namespace qutim_sdk_0_3;

inline SettingsItem *item()
{
	static GeneralSettingsItem<SettingsWidget> item(Settings::Plugin,
													Icon("mail-signature-unknown"),
													QT_TRANSLATE_NOOP("Antispam", "Antispam"));
	return &item;
}

void AntispamPlugin::init()
{
	debug() << Q_FUNC_INFO;
	addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("gorthauer87@yandex.ru"),
			  QLatin1String("sauron.me"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Antispam"),
			QT_TRANSLATE_NOOP("Plugin", "Blocks messages from unknown contacts by question-answer pair"),
			QUTIM_MAKE_VERSION(0, 0, 1, 0));
	setCapabilities(Loadable);
}
bool AntispamPlugin::load()
{
	if (!m_handler)
		m_handler = new Handler;
	MessageHandler::registerHandler(m_handler.data(), 0, MessageHandler::HighPriority + 100500);
	Settings::registerItem(item());
	item()->connect(SIGNAL(saved()), m_handler, SLOT(loadSettings()));
	return true;
}
bool AntispamPlugin::unload()
{
	if (m_handler) {
		m_handler->deleteLater();
		MessageHandler::unregisterHandler(m_handler.data());
	}
	Settings::removeItem(item());
	return true;
}

}

QUTIM_EXPORT_PLUGIN(Antispam::AntispamPlugin)

