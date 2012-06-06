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

#ifndef THEMEMANAGERWRAPPER_H
#define THEMEMANAGERWRAPPER_H

#include <QObject>
#include <qdeclarative.h>
#include <QDir>

namespace MeegoIntegration
{
class ThemeManagerWrapper : public QObject
{
    Q_OBJECT
public:
	explicit ThemeManagerWrapper();
	static void init();
	Q_INVOKABLE QString path(const QString &category, const QString &themeName);
	Q_INVOKABLE QStringList list(const QString &category);
	Q_INVOKABLE void addPath(const QString &path_, const QString &category = QString());
	Q_INVOKABLE QList<QDir> categoryDirs(const QString &category);
};
}

QML_DECLARE_TYPE(MeegoIntegration::ThemeManagerWrapper)

#endif // THEMEMANAGERWRAPPER_H

