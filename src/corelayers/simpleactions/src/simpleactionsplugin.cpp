/****************************************************************************
 *  simpleactionsplugin.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "simpleactionsplugin.h"
#include "simpleactions.h"

namespace Core {
	
SimpleActionsPlugin::SimpleActionsPlugin() :
	m_actions(0)
{

}

	
void SimpleActionsPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Simple Actions"),
			QT_TRANSLATE_NOOP("Plugin", "Default actionset for contacts"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QT_TRANSLATE_NOOP("Author","Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me"));
}

bool SimpleActionsPlugin::load()
{
	Q_ASSERT(!m_actions);
	m_actions = new SimpleActions;
	return true;
}

bool SimpleActionsPlugin::unload()
{
	if (m_actions)
		m_actions->deleteLater();
	return true;
}


	
} // namespace Core

QUTIM_EXPORT_PLUGIN(Core::SimpleActionsPlugin)
