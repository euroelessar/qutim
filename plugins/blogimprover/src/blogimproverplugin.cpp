/****************************************************************************
**
** qutIM - instant messenger
**
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

#include "blogimproverplugin.h"
#include <QScopedPointer>

namespace BlogImprover
{

	using namespace qutim_sdk_0_3;

	void BlogImproverPlugin::init()
	{
		setInfo(QT_TRANSLATE_NOOP("Plugin", "BlogImprover"),
				QT_TRANSLATE_NOOP("Plugin", "Integration with different services of microblogging"),
				PLUGIN_VERSION(0, 1, 0, 0));
		setCapabilities(Loadable);
		addAuthor(QLatin1String("nicoizo"));
	}
	
	bool BlogImproverPlugin::load()
	{

		m_settingsItem.reset(new GeneralSettingsItem<BlogImproverSettings>(
					Settings::Plugin,	QIcon(),
					QT_TRANSLATE_NOOP("Plugin", "BlogImprover")));
		Settings::registerItem(m_settingsItem.data());


		m_handler.reset(new BlogImproverHandler);
		qutim_sdk_0_3::MessageHandler::registerHandler(m_handler.data(),
													   qutim_sdk_0_3::MessageHandler::HighPriority,
													   qutim_sdk_0_3::MessageHandler::HighPriority);
		m_settingsItem.data()->connect(SIGNAL(saved()), m_handler.data(), SLOT(loadSettings()));
		return true;
	}
	
	bool BlogImproverPlugin::unload()
	{
		m_handler.reset(0);
		m_settingsItem.reset(0);
		return true;
	}
	
}

QUTIM_EXPORT_PLUGIN(BlogImprover::BlogImproverPlugin)
