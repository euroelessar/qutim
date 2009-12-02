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

#ifndef XDGENVIRONMENTMAP_H
#define XDGENVIRONMENTMAP_H

#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QProcess>

class XdgEnvironmentMap
{
	Q_DISABLE_COPY(XdgEnvironmentMap)
public:
	static QDir dataHome();
	static QDir configHome();
	static QList<QDir> dataDirs();
	static QList<QDir> configDirs();
private:
	XdgEnvironmentMap();
	~XdgEnvironmentMap();
};

#endif // XDGENVIRONMENTMAP_H
