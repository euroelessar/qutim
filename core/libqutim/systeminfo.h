/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Remko Tronçon <remko@el-tramo.be>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include "libqutim_global.h"
#include <QDir>
#include <QString>

namespace qutim_sdk_0_3
{
struct SystemInfoPrivate;

class LIBQUTIM_EXPORT SystemInfo
{
public:
	enum Type
	{
		WinCE       = 'c',
		Win32       = 'w',
		Linux       = 'l',
		MacOSX      = 'm',
		Symbian     = 's',
		Unix        = 'u'
	};
	enum DirType { ConfigDir, HistoryDir, ShareDir, SystemConfigDir, SystemShareDir };
	static QString getFullName();
	static QString getName();
	static QString getVersion();
	static QString getTimezone();
	static int getTimezoneOffset();
	static QDir getDir(DirType type);
	static QString getPath(DirType type);
	static quint32 getSystemVersionID();
	static quint8 getSystemTypeID();
	static QString systemID2String(quint8 type, quint32 id);
private:
	SystemInfo();
	virtual ~SystemInfo();
};
}

#endif // SYSTEMINFO_H

