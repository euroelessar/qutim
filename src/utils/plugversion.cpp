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

QString plugVersion::toString()
{
	return QString::number(major).append(".").append(QString::number(minor)).append(".").append(QString::number(maintenance));
}

void plugVersion::setVersion(const QString& version)
{
	QStringList versionList = version.split(".");
	if (versionList.length()<2)
		return;
	major = versionList.at(0).toUShort();
	minor = versionList.at(1).toUShort();
	if (versionList.length()==2)
		maintenance = 0;
	else
		maintenance = versionList.at(2).toUShort();
}

void plugVersion::setXYZ(ushort major, ushort minor, ushort maintenance)
{
	this->major = major;
	this->minor = minor;
	this->maintenance = maintenance;
}

bool plugVersion::isValid()
{
	return (major!=0)||(maintenance!=0)||(minor!=0); //! если хоть одно из значений не 0, тогда версия имеет право на существование
}

bool plugVersion::operator==(const plugVersion &plug_version)
{
	return (this->major==plug_version.major)&&(this->minor==plug_version.minor)&&(this->maintenance==plug_version.maintenance);
}

bool plugVersion::operator!=(const plugVersion& plug_version)
{
	return (this->major!=plug_version.major)||(this->minor!=plug_version.minor)||(this->maintenance!=plug_version.maintenance);
}

bool plugVersion::operator>(const plugVersion& plug_version)
{
	if (this->major!=plug_version.major)
		return this->major>plug_version.major;
	else
		if (this->minor!=plug_version.minor)
			return this->minor>plug_version.minor;
		else
			return this->maintenance>plug_version.maintenance;
}

bool plugVersion::operator<(const plugVersion& plug_version)
{
	if (this->major!=plug_version.major)
		return this->major<plug_version.major;
	else
		if (this->minor!=plug_version.minor)
			return this->minor<plug_version.minor;
		else
			return this->maintenance<plug_version.maintenance;
}

