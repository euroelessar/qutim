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

QString XdgThemeChooserGnome2::getThemeId() const
{
    QString themeName;

    QProcess process;
    process.start("gconftool-2 -g /desktop/gnome/interface/icon_theme", QIODevice::ReadOnly);

    if (process.waitForStarted())
    {
        QTextStream stream(&process);

        while (process.waitForReadyRead())
            themeName += stream.readAll();

        themeName = themeName.trimmed();
        process.close();
        return themeName;
    }

    // Attempt to read gtkrc
    QFile file(QDir::home().absoluteFilePath(".gtkrc-2.0"));

    if (file.exists())
    {
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        QRegExp exp("^\\s*gtk-icon-theme-name\\s*=(.*)");
        QString str;

        while (!(str = stream.readLine()).isEmpty())
        {
            if (exp.indexIn(str) != -1)
            {
                themeName = exp.cap(1).trimmed();

                if ((themeName.startsWith('"') && themeName.endsWith('"')) ||
                        (themeName.startsWith('\'') && themeName.endsWith('\'')))
                {
                    themeName = themeName.mid(1, themeName.length() - 2).trimmed();
                }

                return themeName;
            }
        }
    }

    return "gnome";
}

QString XdgThemeChooserKde4::getThemeId() const
{
    XdgEnvironmentMap envMap;
    QDir kdehome(envMap.value("KDEHOME", QDir::home().absoluteFilePath(".kde")));

    if (kdehome.exists())
    {
        QString config = kdehome.absoluteFilePath("share/config/kdeglobals");

        if (QFile::exists(config))
        {
            QSettings settings(config, QSettings::IniFormat);
            return settings.value("Icons/Theme").toString();
        }
    }

    return "oxygen";
}
