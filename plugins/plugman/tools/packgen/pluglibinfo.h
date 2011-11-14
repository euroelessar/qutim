/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef PLUGLIBINFO_H
#define PLUGLIBINFO_H

#include <QString>
#include "utils/plugversion.h"
#include "plugpackage.h"

class plugLibInfo
{
public:
	enum Type { QtLib, QtPlugin, QutimPlugin, Library, Invalid };
	plugLibInfo(const QString &lib_path);
	inline Type type() const { return m_type; }
	inline bool isValid() const { return type() != Invalid; }
	packageInfo toInfo() const;
private:
	Type m_type;
	QString m_magic_type;
	QString m_lib_path;
	QString m_description;
	QString m_name;
	QString m_build_key;
	bool m_debug;
	plugVersion m_qt_version;
};

#endif // PLUGLIBINFO_H

