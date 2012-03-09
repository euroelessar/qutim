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

#include "authdialogplugin.h"
#include "authservice.h"

namespace Core {

using namespace qutim_sdk_0_3;

void AuthDialogPlugin::init()
{
	qutim_sdk_0_3::ExtensionIcon icon(QLatin1String(""));
	qutim_sdk_0_3::LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Authorization dialog");
	qutim_sdk_0_3::LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Simple authorization dialog");
	setInfo(name, description, QUTIM_VERSION, icon);
	addAuthor(QLatin1String("sauron"));
	addExtension<Core::AuthService>(name, description, icon);
}

bool AuthDialogPlugin::load()
{
	return true;
}

bool AuthDialogPlugin::unload()
{
	return true;
}

} // namespace Core

QUTIM_EXPORT_PLUGIN(Core::AuthDialogPlugin)

