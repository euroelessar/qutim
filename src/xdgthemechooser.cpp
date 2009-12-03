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
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include "xdgthemechooser.h"
#include "xdgenvironmentmap.h"

XdgThemeChooser::~XdgThemeChooser()
{
}

QString XdgThemeChooserGnome2::getThemeId() const
{
    QString themeName;

    QProcess process;
    process.start(QLatin1String("gconftool-2 -g /desktop/gnome/interface/icon_theme"), QIODevice::ReadOnly);

    if (process.waitForStarted()) {
        QTextStream stream(&process);

        while (process.waitForReadyRead())
            themeName += stream.readAll();

        themeName = themeName.trimmed();
        process.close();
        return themeName;
    }

    // Attempt to read gtkrc
    QFile file(QDir::home().absoluteFilePath(QLatin1String(".gtkrc-2.0")));

    if (file.exists()) {
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        QRegExp exp(QLatin1String("^\\s*gtk-icon-theme-name\\s*=(.*)"));
        QString str;

        while (!(str = stream.readLine()).isEmpty()) {
            if (exp.indexIn(str) != -1) {
                themeName = exp.cap(1).trimmed();

                if ((themeName.startsWith('"') && themeName.endsWith('"'))
                    || (themeName.startsWith('\'') && themeName.endsWith('\''))) {
                    themeName = themeName.mid(1, themeName.length() - 2).trimmed();
                }

                return themeName;
            }
        }
    }

    return QLatin1String("gnome");
}

QString XdgThemeChooserKde4::getThemeId() const
{
    QDir kdeHome;
    {
        QByteArray env = qgetenv("KDEHOME");

        if (env.isEmpty()) {
            kdeHome = QDir::home();
            // We should try both ~/.kde and ~/.kde4
            if (!(kdeHome.cd(QLatin1String(".kde")) || kdeHome.cd(QLatin1String(".kde4"))))
                return QLatin1String("oxygen");
        }
        else {
            kdeHome = QString::fromLocal8Bit(env, env.size());
        }
    }

    if (kdeHome.exists()) {
        QString config = kdeHome.absoluteFilePath(QLatin1String("share/config/kdeglobals"));

        if (QFile::exists(config)) {
            QSettings settings(config, QSettings::IniFormat);
            return settings.value(QLatin1String("Icons/Theme")).toString();
        }
    }

    return QLatin1String("oxygen");
}
