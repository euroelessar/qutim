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
}

bool SimpleActionsPlugin::load()
{
	Q_ASSERT(!m_actions);
	m_actions = new SimpleActions;
	return true;
}

bool SimpleActionsPlugin::unload()
{
	Q_ASSERT(m_actions);
	m_actions->deleteLater();
	return true;
}


	
} // namespace Core

QUTIM_EXPORT_PLUGIN(Core::SimpleActionsPlugin)
