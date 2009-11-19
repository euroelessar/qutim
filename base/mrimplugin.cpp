/****************************************************************************
 *  mrimplugin.cpp
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
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

#include "mrimplugin.h"
#include "mrimprotocol.h"

#define MRIM_MAJOR_VER 0
#define MRIM_MINOR_VER 1
#define MRIM_SECMINOR_VER 0
#define MRIM_PATCH_VER 0

MrimPlugin::MrimPlugin()
{
    qDebug("%s", Q_FUNC_INFO);
}

void MrimPlugin::init()
{
    qDebug("%s", Q_FUNC_INFO);
    const char *info = QT_TRANSLATE_NOOP("Plugin", "Module-based realization of Mail.Ru IM protocol");
    const char *plugName = QT_TRANSLATE_NOOP("Plugin", "Mrim");

    m_info = PluginInfo(plugName, info,
                        PLUGIN_VERSION(MRIM_MAJOR_VER, MRIM_MINOR_VER, MRIM_SECMINOR_VER, MRIM_PATCH_VER));

    m_info.addAuthor(QT_TRANSLATE_NOOP("Author", "Peter Rusanov"),
                     QT_TRANSLATE_NOOP("Task", "Author"),
                     QLatin1String("peter.rusanov@gmail.com"));

    m_extensions << ExtensionInfo(plugName,info,new GeneralGenerator<MrimProtocol>());
}

bool MrimPlugin::load()
{
    qDebug("%s", Q_FUNC_INFO);
    return true;
}

bool MrimPlugin::unload()
{
    qDebug("%s", Q_FUNC_INFO);
    return false;
}

QUTIM_EXPORT_PLUGIN(MrimPlugin)
