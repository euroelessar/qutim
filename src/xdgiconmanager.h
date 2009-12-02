/*
    Copyright (C) 2009 Maia Kozheva <sikon@ubuntu.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef XDGICONMANAGER_H
#define XDGICONMANAGER_H

#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QRegExp>
#include <QtCore/QSharedData>
#include "xdgenvironmentmap.h"
#include "xdgicontheme.h"
#include "xdgthemechooser.h"

class XdgIconManagerPrivate;

class XdgIconManager
{
public:
    XdgIconManager();
	virtual ~XdgIconManager();

	XdgIconManager(const XdgIconManager &other);
	XdgIconManager &operator =(const XdgIconManager &other);

    void clearRules();
    void installRule(const QRegExp& regexp, const XdgThemeChooser *chooser);
    const XdgIconTheme *defaultTheme(const QString& xdgSession = QString()) const;
    const XdgIconTheme *themeByName(const QString& themeName) const;
    const XdgIconTheme *themeById(const QString& themeId) const;

	QStringList themeNames() const;
	QStringList themeIds() const;
protected:
	QSharedDataPointer<XdgIconManagerPrivate> d;
};

#endif // XDGICONMANAGER_H
