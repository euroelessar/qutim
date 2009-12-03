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
#include "xdgiconmanager_p.h"

namespace
{
    XdgThemeChooserGnome2 gnomeChooser;
    XdgThemeChooserKde4 kdeChooser;
}

XdgIconManager::XdgIconManager() : d(new XdgIconManagerPrivate)
{
    d->rules.insert(QLatin1String("gnome"), &gnomeChooser);
    d->rules.insert(QLatin1String("kde"), &kdeChooser);
    d->init();
}

XdgIconManager::~XdgIconManager()
{
}

XdgIconManager::XdgIconManager(const XdgIconManager &other) : d(other.d)
{
}

XdgIconManager &XdgIconManager::operator =(const XdgIconManager &other)
{
    d = other.d;
}

void XdgIconManagerPrivate::init()
{
    // Identify base directories
    QVector<QDir> basedirs;
    QDir basedir(QDir::home().absoluteFilePath(QLatin1String(".icons")));

    if(basedir.exists())
        basedirs.append(basedir);

    QList<QDir> datadirs = XdgEnvironmentMap::dataDirs();

    foreach (QDir dir, datadirs) {
        basedir = dir.absoluteFilePath(QLatin1String("icons"));

        if(basedir.exists())
            basedirs.append(basedir);
    }

    basedir = QLatin1String("/usr/share/pixmaps");

    if (basedir.exists())
        basedirs.append(basedir);

    // Build theme list
    foreach (QDir dir, basedirs) {
        QDirIterator subdirs(dir);

        while (subdirs.hasNext()) {
            QFileInfo subdir(subdirs.next());

            if (!subdir.isDir())
                continue;

            QString index = QDir(subdir.canonicalFilePath()).absoluteFilePath(QLatin1String("index.theme"));

            if (QFileInfo(index).exists()) {
                QSettings settings(index, QSettings::IniFormat);
                QString name = settings.value(QLatin1String("Icon Theme/Name")).toString();

                if (!name.isEmpty()) {
                    QMap<QString, XdgIconTheme *>::const_iterator it = themes.constFind(name);
                    XdgIconTheme *theme;
                    if (it == themes.constEnd()) {
                        theme = new XdgIconTheme(basedirs, subdir.fileName(), index);
                        themes.insert(name, theme);
                    } else
                        theme = it.value();

                    themeIdMap.insert(subdir.fileName(), theme);
                }
            }
        }
    }

    const XdgIconTheme *hicolor = themeIdMap.value(QLatin1String("hicolor"));

    // Resolve dependencies
    for(QMap<QString, XdgIconTheme*>::iterator it = themes.begin(); it != themes.end(); ++it) {
        XdgIconTheme &theme = *it.value();

        if (theme.id() == QLatin1String("hicolor"))
            continue;

        if (theme.parentNames().isEmpty()) {
            theme.addParent(hicolor);
            continue;
        }

        foreach (QString parent, theme.parentNames()) {
            const XdgIconTheme *parentTheme = themeIdMap.value(parent);
            if(parentTheme)
                theme.addParent(parentTheme);
        }
    }
}

void XdgIconManager::clearRules()
{
    d->rules.clear();
}

void XdgIconManager::installRule(const QRegExp& regexp, const XdgThemeChooser *chooser)
{
    d->rules.insert(regexp.pattern(), chooser);
}

const XdgIconTheme *XdgIconManager::defaultTheme(const QString& xdgSession) const
{
    QByteArray env = qgetenv("DESKTOP_SESSION");
    QString session = QString::fromLocal8Bit(env, env.size());

    const XdgThemeChooser *chooser = &kdeChooser;

    QHash<QString, const XdgThemeChooser *>::const_iterator it;

    for (it = d->rules.begin(); it != d->rules.end(); ++it) {
        // FIXME: Is it really needed to use regular expressions here?
        if(QRegExp(it.key()).indexIn(session) != -1) {
            chooser = it.value();
            break;
        }
    }

    return themeById(chooser->getThemeId());
}

const XdgIconTheme *XdgIconManager::themeByName(const QString& themeName) const
{
    return d->themes.value(themeName, 0);
}

const XdgIconTheme *XdgIconManager::themeById(const QString& themeName) const
{
    return d->themeIdMap.value(themeName);
}

QStringList XdgIconManager::themeNames() const
{
    return QStringList(d->themes.keys());
}

QStringList XdgIconManager::themeIds() const
{
    return QStringList(d->themeIdMap.keys());
}
