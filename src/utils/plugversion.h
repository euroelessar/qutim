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

#include <QString>
#ifndef PLUGVERSION_H
#define PLUGVERSION_H

class plugVersion
{
	//! WARNING version format is x.y.z where x is major version string, y - minor, z - maintenance
public:
	plugVersion (const QString &version);
	void setVersion (const QString &version);
	void setXYZ (ushort major,ushort minor,ushort maintenance);
	QString getVersion ();
	bool isValid ();
	bool operator == (const plugVersion& plug_version);
	bool operator != (const plugVersion& plug_version);
	bool operator >  (const plugVersion& plug_version);
	bool operator <  (const plugVersion& plug_version);
private:
	ushort major;
	ushort minor;
	ushort maintenance;
};

#endif // PLUGVERSION_H
