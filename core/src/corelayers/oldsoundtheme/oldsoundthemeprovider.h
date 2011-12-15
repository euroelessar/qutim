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

#ifndef OLDSOUNDTHEMEPROVIDER_H
#define OLDSOUNDTHEMEPROVIDER_H

#include <qutim/sound.h>

using namespace qutim_sdk_0_3;

namespace Core
{
class OldSoundThemeProvider : public SoundThemeProvider
{
public:
    OldSoundThemeProvider(const QString &name, const QString &path, QString variant);
	virtual bool setSoundPath(Notification::Type sound, const QString &file);
	virtual QString soundPath(Notification::Type sound);
	virtual QString themeName();
	virtual bool saveTheme();
protected:
	QMap<Notification::Type, QString> m_map;
	QString m_filePath;
	QString m_themeName;
};
}

#endif // OLDSOUNDTHEMEPROVIDER_H

