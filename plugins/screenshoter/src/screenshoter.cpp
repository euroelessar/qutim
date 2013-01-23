/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "screenshoter.h"
#include "qutim/shortcut.h"
#include "qutim/adiumchat/abstractchatwidget.h"

using namespace qutim_sdk_0_3;

Screenshoter::Screenshoter()
{
    m_shortcut = 0;
}

void Screenshoter::init()
{
    setInfo(QT_TRANSLATE_NOOP("Plugin", "Screenshoter"),
            QT_TRANSLATE_NOOP("Plugin", "Screenshot and upload to image hosting"),
            PLUGIN_VERSION(0, 1, 0, 0));
    setCapabilities(Loadable);
    addAuthor(QLatin1String("trett"));
    GlobalShortcut::registerSequence("screen", QT_TRANSLATE_NOOP("Screenshoter", "Printscreen"),
                                     QT_TRANSLATE_NOOP("Screenshoter", "Screenshoter"),
                                     QKeySequence("Ctrl+D")
                                     );
}

bool Screenshoter::load()
{
    if (!m_shortcut) {
        m_shortcut = new GlobalShortcut("screen",this);
        connect(m_shortcut,SIGNAL(activated()),this,SLOT(initShoter()));
    }
    return true;
}

bool Screenshoter::unload()
{
    delete m_shoter;
    return true;
}

void Screenshoter::initShoter()
{
    m_shoter = new Shoter();
    m_shoter->readSettings();
    m_shoter->show();
}

QUTIM_EXPORT_PLUGIN(Screenshoter)
