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

namespace
{
    inline QList<QDir> splitDirList(const QString& str)
    {
        QList<QDir> list;

		foreach(QString str, str.split(QLatin1Char(':')))
			list.append(QDir(str));

        return list;
    }

	inline QString getValue(const char *varName, const QString &defValue)
	{
		QByteArray env = qgetenv(varName);
		return env.isEmpty() ? defValue : QString::fromLocal8Bit(env.constData(), env.size());
	}
}

XdgEnvironmentMap::XdgEnvironmentMap()
{
}

XdgEnvironmentMap::~XdgEnvironmentMap()
{
}

QDir XdgEnvironmentMap::dataHome()
{
	return QDir(getValue("XDG_DATA_HOME",
						 QDir::home().absoluteFilePath(QLatin1String(".share/locale"))));
}

QDir XdgEnvironmentMap::configHome()
{
	return QDir(getValue("XDG_CONFIG_HOME",
						 QDir::home().absoluteFilePath(QLatin1String(".config"))));
}

QList<QDir> XdgEnvironmentMap::dataDirs()
{
	return splitDirList(getValue("XDG_DATA_DIRS",
								 QLatin1String("/usr/local/share:/usr/share")));
}

QList<QDir> XdgEnvironmentMap::configDirs()
{
	return splitDirList(getValue("XDG_CONFIG_DIRS",
								 QDir::home().absoluteFilePath(QLatin1String(".share/locale"))));
}
