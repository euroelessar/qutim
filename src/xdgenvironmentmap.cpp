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

#include "xdgenvironmentmap.h"

namespace {
    inline QList<QDir> splitDirList(const QString& str)
    {
        QList<QDir> list;

        foreach(QString str, str.split(':'))
        {
            list.append(QDir(str));
        }

        return list;
    }
}

XdgEnvironmentMap::XdgEnvironmentMap(QProcess *process)
 : _process(process)
{
    reload();
}

void XdgEnvironmentMap::reload()
{
    clear();

    QStringList env = _process ? _process->environment() : QProcess::systemEnvironment();

    for(QStringList::iterator it = env.begin(); it != env.end(); ++it)
    {
        QStringList list = it->split('=');
        insert(list.at(0), list.at(1));
    }
}

QDir XdgEnvironmentMap::dataHome() const
{
    return QDir(value("XDG_DATA_HOME", QDir::home().absoluteFilePath(".share/locale")));
}

QDir XdgEnvironmentMap::configHome() const
{
    return QDir(value("XDG_CONFIG_HOME", QDir::home().absoluteFilePath(".config")));
}

QList<QDir> XdgEnvironmentMap::dataDirs() const
{
    return splitDirList(value("XDG_DATA_DIRS", "/usr/local/share:/usr/share"));
}

QList<QDir> XdgEnvironmentMap::configDirs() const
{
    return splitDirList(value("XDG_CONFIG_DIRS", QDir::home().absoluteFilePath(".share/locale")));
}
