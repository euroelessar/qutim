/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>
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

#include "awn.h"
#include "awnservice.h"

#include <qutim/icon.h>

AWNPlugin::AWNPlugin ()
{
}

void AWNPlugin::init ()
{
    qutim_sdk_0_3::ExtensionIcon icon("info");
    addAuthor(QT_TRANSLATE_NOOP("Author", "Stanislav (proDOOMman) Kosolapov"),
              QT_TRANSLATE_NOOP("Task", "Developer"),
              QLatin1String("prodoomman@shell.tor.hu")
              );
    setInfo(QT_TRANSLATE_NOOP("Plugin", "AWN"),
            QT_TRANSLATE_NOOP("Plugin", "Avant window navigator dock integration"),
            PLUGIN_VERSION(0, 0, 1, 0),
            icon
            );
    addExtension<AWNService>(QT_TRANSLATE_NOOP("Plugin", "AWN"),
                             QT_TRANSLATE_NOOP("Plugin", "Avant window navigator dock integration")
                             );
}

bool AWNPlugin::load ()
{
    return true;
}

bool AWNPlugin::unload ()
{
    return false;
}

QUTIM_EXPORT_PLUGIN(AWNPlugin)

