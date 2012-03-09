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
	addAuthor(QLatin1String("sauron"));
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

