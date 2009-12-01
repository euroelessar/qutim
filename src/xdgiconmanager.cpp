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

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include "xdgiconmanager.h"

namespace
{
    XdgThemeChooserGnome2 gnomeChooser;
    XdgThemeChooserKde4 kdeChooser;
}

XdgIconManager::XdgIconManager()
{
    _rules.insert("gnome", &gnomeChooser);
    _rules.insert("kde", &kdeChooser);
    init();
}

XdgIconManager::XdgIconManager(const XdgIconManager& other)
        : _rules(other._rules)
{
    init();
}

void XdgIconManager::init()
{
    // Identify base directories
    QVector<QDir> basedirs;
    QDir basedir(QDir::home().absoluteFilePath(".icons"));

    if(basedir.exists())
    {
        basedirs.append(basedir);
    }

    QList<QDir> datadirs = _envMap.dataDirs();

    foreach(QDir dir, datadirs)
    {
        basedir = dir.absoluteFilePath("icons");

        if(basedir.exists())
        {
            basedirs.append(basedir);
        }
    }

    basedir = "/usr/share/pixmaps";

    if(basedir.exists())
    {
        basedirs.append(basedir);
    }

    // Build theme list
    foreach(QDir dir, basedirs)
    {
        QDirIterator subdirs(dir);

        while(subdirs.hasNext())
        {
            QFileInfo subdir(subdirs.next());

            if(!subdir.isDir())
            {
                continue;
            }

            QString index = QDir(subdir.canonicalFilePath()).absoluteFilePath("index.theme");

            if(QFileInfo(index).exists())
            {
                QSettings settings(index, QSettings::IniFormat);
                QString name = settings.value("Icon Theme/Name").toString();

                if(!name.isEmpty())
                {
                    if (!_themes.contains(name))
                    {
                        _themes.insert(name, XdgIconTheme(basedirs, subdir.fileName(), index));
                    }

                    _themeIdMap.insert(subdir.fileName(), &_themes.find(name).value());
                }
            }
        }
    }

    const XdgIconTheme *hicolor = themeById("hicolor");

    // Resolve dependencies
    for(QMap<QString, XdgIconTheme>::iterator it = _themes.begin(); it != _themes.end(); ++it)
    {
        XdgIconTheme& theme = it.value();

        if (theme.id() == "hicolor")
        {
            continue;
        }

        if(theme.parentNames().isEmpty())
        {
            theme.addParent(hicolor);
            continue;
        }

        foreach(QString parent, theme.parentNames())
        {
            theme.addParent(themeById(parent));
        }
    }
}

void XdgIconManager::clearRules()
{
    _rules.clear();
}

void XdgIconManager::installRule(const QRegExp& regexp, const XdgThemeChooser *chooser)
{
    _rules.insert(regexp.pattern(), chooser);
}

const XdgIconTheme *XdgIconManager::defaultTheme(const QString& xdgSession) const
{
    QString session = _envMap.value("DESKTOP_SESSION");
    const XdgThemeChooser *chooser = &kdeChooser;

    for (QHash<QString, const XdgThemeChooser *>::const_iterator it = _rules.begin(); it != _rules.end(); ++it)
    {
        if(QRegExp(it.key()).indexIn(session) != -1)
        {
            chooser = it.value();
            break;
        }
    }

    return themeById(chooser->getThemeId());
}

const XdgIconTheme *XdgIconManager::themeByName(const QString& themeName) const
{
    return _themes.contains(themeName) ? &_themes.find(themeName).value() : 0;
}

const XdgIconTheme *XdgIconManager::themeById(const QString& themeName) const
{
    return _themeIdMap.value(themeName);
}
