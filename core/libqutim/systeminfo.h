/*****************************************************************************
 System Info

 Copyright (c) 2007-2008 by Remko Tronçon
	  2008-2010 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include "libqutim_global.h"
#include <QDir>

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
