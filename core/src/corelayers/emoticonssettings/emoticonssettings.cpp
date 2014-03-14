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

#include "emoticonssettings.h"
#include <qutim/libqutim_global.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Core
{
    using namespace qutim_sdk_0_3;

	EmoticonsSettings::EmoticonsSettings()
	{
        SettingsItem *item = new QmlSettingsItem(QStringLiteral("emoticonssettings"),
                                                 Settings::Appearance,
                                                 Icon("face-smile"),
                                                 QT_TRANSLATE_NOOP("Settings","Emoticons"));
		Settings::registerItem(item);
        deleteLater();
    }
}

