/*****************************************************************************
	System Info

	Copyright (c) 2007-2008 by Remko Tronçon
						2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

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

#include <QString>

class SystemInfo
{
public:
	static SystemInfo &instance();
	const QString& os() const { return m_os_str; }
	int timezoneOffset() const { return m_timezone_offset; }
	const QString& timezoneString() const { return m_timezone_str; }
	void getSystemInfo(QString &os, QString &timezone, int &timezone_offset);
	inline void getSystemInfo(QString &name, QString &version) { name = m_name; version = m_version; }

private:
	SystemInfo();
	int m_timezone_offset;
	QString m_timezone_str;
	QString m_os_str;
	QString m_name;
	QString m_version;
};

#endif
