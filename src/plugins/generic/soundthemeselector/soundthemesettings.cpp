/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "soundthemesettings.h"
#include "soundthemeselector.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace Core
{
SoundThemeSettings::SoundThemeSettings(QObject *parent) :
	QObject(parent)
{
	GeneralSettingsItem<SoundThemeSelector> *item = new GeneralSettingsItem<SoundThemeSelector>(Settings::Appearance, Icon("speaker"), QT_TRANSLATE_NOOP("Settings","Sounds"));
	Settings::registerItem(item);
	deleteLater();
}

}

