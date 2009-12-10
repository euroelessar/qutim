/*
    Copyright (C) 2009 Nigmatullin Ruslan <euroelessar@ya.ru>

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

#ifndef XDGICONMANAGER_P_H
#define XDGICONMANAGER_P_H

#include "xdgiconmanager.h"
#include "xdgicontheme_p.h"
#include <QtCore/QSet>

inline uint qHash(const QRegExp &regexp)
{ return qHash(regexp.pattern()); }

class XdgIconManagerPrivate : public QSharedData
{
public:
    XdgIconManagerPrivate() {}
    XdgIconManagerPrivate(const XdgIconManagerPrivate &o)
            : rules(o.rules), themes(o.themes), themeIdMap(o.themeIdMap) {}
    ~XdgIconManagerPrivate();
    QHash<QRegExp, XdgThemeChooser> rules;
    mutable QMap<QString, XdgIconTheme *> themes;
    mutable QMap<QString, XdgIconTheme *> themeIdMap;

    void init(const QList<QDir> &appDirs);
};

XdgIconManagerPrivate::~XdgIconManagerPrivate()
{
    // There sometimes equal values for different keys, i.e. because of fallback
    QSet<XdgIconData *> allData;
    foreach (XdgIconTheme *theme, themes)
        allData |= QSet<XdgIconData *>::fromList(theme->p->cache.values());
    qDeleteAll(allData);

    // FIXME: May be it will be better to carry all XdgIconTheme's in some list?..
    QSet<XdgIconTheme *> allThemes;
    allThemes |= QSet<XdgIconTheme *>::fromList(allThemes.values());
    allThemes |= QSet<XdgIconTheme *>::fromList(themeIdMap.values());
    qDeleteAll(allThemes);
}

#endif // XDGICONMANAGER_P_H
