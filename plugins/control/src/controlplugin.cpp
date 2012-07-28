/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "controlplugin.h"
#include "rostermanager.h"

namespace Control {

using namespace qutim_sdk_0_3;

ControlPlugin::ControlPlugin()
{
}

void ControlPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Control"),
			QT_TRANSLATE_NOOP("Plugin", "Controls user by sending messages and roster to remote server"),
			PLUGIN_VERSION(0, 0, 1, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("euroelessar"));
//	addExtension(QT_TRANSLATE_NOOP("Plugin", "Controlled Roster"),
//				 QT_TRANSLATE_NOOP("Plugin", "Controlls roster by remote server"),
//				 new GeneralGenerator<RosterManager>(),
//				 ExtensionIcon(""));
}

bool ControlPlugin::load()
{
	m_roster.reset(new RosterManager);
	return true;
}

bool ControlPlugin::unload()
{
	m_roster.reset(0);
	return true;
}

} // namespace Control

QUTIM_EXPORT_PLUGIN(Control::ControlPlugin)
