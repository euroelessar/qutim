/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "plugversion.h"
#include <QStringList>

plugVersion::plugVersion(const QString& version)
{
    setVersion(version);
}

plugVersion::plugVersion(ushort ver1, ushort ver2, ushort ver3, ushort ver4, ushort ver5, ushort ver6)
{
	m_version << ver1 << ver2 << ver3 << ver4 << ver5 << ver6;
	standartize();
}

QString plugVersion::toString() const
{
	if(m_version.isEmpty())
		return "0.0.0";
	QString version;
	for(int i = 0; i < m_version.size(); i++)
	{
		version += QString::number(m_version.at(i));
		if(i + 1 < m_version.size())
			version += ".";
	}
	if(m_version.size() < 2)
		version += ".0";
	if(m_version.size() < 3)
		version += ".0";
	return version;
}

void plugVersion::setVersion(const QString& version)
{
    QStringList versionList = version.split(".");
	m_version.clear();
	for(int i = 0; i < versionList.size(); i++)
		m_version << versionList.at(i).toUShort();
	standartize();
}

void plugVersion::setVersion (ushort ver1, ushort ver2, ushort ver3, ushort ver4, ushort ver5, ushort ver6)
{
	m_version.clear();
	m_version << ver1 << ver2 << ver3 << ver4 << ver5 << ver6;
	standartize();
}

void plugVersion::standartize()
{
	for(int i = m_version.size() - 1; i >= 0; i--)
	{
		if(m_version[i] == 0)
			m_version.remove(i);
		else
			return;
	}
}

bool plugVersion::isValid() const
{
	return !m_version.isEmpty();
}

bool plugVersion::operator==(const plugVersion &that) const
{
	for(int i = 0; i < qMax(m_version.size(), that.m_version.size()); i++)
		if(value(i) != that.value(i))
			return false;
	return true;
}

bool plugVersion::operator!=(const plugVersion& that) const
{
	return !operator ==(that);
}

bool plugVersion::operator>(const plugVersion& that) const
{
	for(int i = 0; i < qMax(m_version.size(), that.m_version.size()); i++)
		if(value(i) != that.value(i))
			return value(i) > that.value(i);
	return false;
}

bool plugVersion::operator<(const plugVersion& that) const
{
	for(int i = 0; i < qMax(m_version.size(), that.m_version.size()); i++)
		if(value(i) != that.value(i))
			return value(i) < that.value(i);
	return false;
}

