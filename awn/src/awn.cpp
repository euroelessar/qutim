/*

    Copyright (c) 2010 by Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

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
