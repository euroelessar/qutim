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
#include "xdgenvironment.h"
#include "xdgiconmanager_p.h"

XdgIconManager::XdgIconManager(const QList<QDir> &appDirs) : d(new XdgIconManagerPrivate)
{
    d->rules.insert(QRegExp(QLatin1String("gnome"), Qt::CaseInsensitive), &xdgGetGnomeTheme);
    d->rules.insert(QRegExp(QLatin1String("kde"), Qt::CaseInsensitive), &xdgGetKdeTheme);
    d->init(appDirs);
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
	return *this;
}

void XdgIconManagerPrivate::init(const QList<QDir> &appDirs)
{
    // Identify base directories
    QVector<QDir> basedirs;
    QDir basedir(QDir::home());

    if(basedir.cd(QLatin1String(".icons")) && !basedirs.contains(basedir))
        basedirs.append(basedir);

    QList<QDir> datadirs = XdgEnvironment::dataDirs();
    datadirs << appDirs;

    QString iconsStr(QLatin1String("icons"));

    foreach (QDir dir, datadirs) {
        dir.makeAbsolute();
        if (dir.cd(iconsStr) && !basedirs.contains(dir))
            basedirs.append(dir);
    }

    basedir = QLatin1String("/usr/share/pixmaps");

    if (basedir.exists() && !basedirs.contains(basedir))
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

void XdgIconManager::installRule(const QRegExp &regexp, XdgThemeChooser chooser)
{
    d->rules.insert(regexp, chooser);
}

const XdgIconTheme *XdgIconManager::defaultTheme(const QString &xdgSession) const
{
    // What is xdgSession?.. Can't understand logic
    Q_UNUSED(xdgSession);

    XdgThemeChooser chooser = 0;
    if (!qgetenv("KDE_FULL_SESSION").isEmpty())
        chooser = &xdgGetKdeTheme;
    else if (!qgetenv("GNOME_DESKTOP_SESSION_ID").isEmpty())
        chooser = &xdgGetGnomeTheme;

    QByteArray env = qgetenv("DESKTOP_SESSION");
    QString session = QString::fromLocal8Bit(env, env.size());

    QHash<QRegExp, XdgThemeChooser>::const_iterator it;

    for (it = d->rules.begin(); it != d->rules.end(); ++it) {
        // FIXME: Is it really needed to use regular expressions here?
        if(it.key().indexIn(session) != -1) {
            chooser = it.value();
            break;
        }
    }

	return themeById((chooser ? *chooser : xdgGetKdeTheme)());
}

const XdgIconTheme *XdgIconManager::themeByName(const QString &themeName) const
{
    return d->themes.value(themeName, 0);
}

const XdgIconTheme *XdgIconManager::themeById(const QString &themeName) const
{
    return d->themeIdMap.value(themeName, 0);
}

QStringList XdgIconManager::themeNames() const
{
    return QStringList(d->themes.keys());
}

QStringList XdgIconManager::themeIds() const
{
    return QStringList(d->themeIdMap.keys());
}
