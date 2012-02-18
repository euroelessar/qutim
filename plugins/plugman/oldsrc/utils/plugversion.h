/****************************************************************************
**
** qutIM - instant messenger
**
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

#ifndef PLUGVERSION_H
#define PLUGVERSION_H

#include <QString>
#include <QVector>

class plugVersion
{
	//! WARNING version format is x.y.z where x is major version string, y - minor, z - maintenance
public:
	plugVersion (const QString &version);
	plugVersion (ushort ver1 = 0, ushort ver2 = 0, ushort ver3 = 0, ushort ver4 = 0, ushort ver5 = 0, ushort ver6 = 0);
	void setVersion (const QString &version);
	void setVersion (ushort ver1, ushort ver2 = 0, ushort ver3 = 0, ushort ver4 = 0, ushort ver5 = 0, ushort ver6 = 0);
	QString toString() const;
	bool isValid() const;
	bool operator == (const plugVersion& plug_version) const;
	bool operator != (const plugVersion& plug_version) const;
	bool operator >  (const plugVersion& plug_version) const;
	bool operator <  (const plugVersion& plug_version) const;
private:
	void standartize();
	inline ushort value(int index) const { return m_version.size() > index ? m_version.at(index) : 0; }
	QVector<ushort> m_version;
};

#endif // PLUGVERSION_H

