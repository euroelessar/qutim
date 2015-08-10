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

#ifndef ICONSLOADERIMPL_H
#define ICONSLOADERIMPL_H

#include <qutim/iconloader.h>
#include <qutim/settingswidget.h>
#include <qutim/settingslayer.h>
#include <QComboBox>
#include <qutim/config.h>
#include <QSet>

using namespace qutim_sdk_0_3;

namespace Core
{

class IconLoaderImpl : public IconLoader
{
	Q_OBJECT
public:
	IconLoaderImpl();
	~IconLoaderImpl();

	QString getMissingIcons() const;
protected:
	QIcon doLoadIcon(const QString &name);
	QMovie *doLoadMovie(const QString &name);
	QString doIconPath(const QString &name, uint iconSize);
	QString doMoviePath(const QString &name, uint iconSize);

private:
	QString m_defaultTheme;
	QHash<QString, QString> m_fallbackIcons;
	ConfigValue<bool> m_defaultEnabled;
	ConfigValue<bool> m_nastyHack;
};

class IconsList : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString iconsList READ iconsList)

public:
	IconsList(){}
	~IconsList(){}

	static QSet<QString> missingIcons;

	QString iconsList() const
	{
		return IconsList::missingIcons.toList().join("\n");
	}

};

namespace QtIcons {

void registerTypes();

}

}

#endif // ICONSLOADERIMPL_H

