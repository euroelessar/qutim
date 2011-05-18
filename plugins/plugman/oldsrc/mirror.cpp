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

#include "mirror.h"

bool mirrorInfo::isValid()
{
	if (this->name.isEmpty())
		return false;
	return this->url.isValid();
}


QString mirrorInfo::toJSON()
{
	//simple and fast method, but dirty
	QString json;
	json += QString("  \"name\": \"%1\" , \n").arg(this->name);
	json += QString("  \"url\": \"%1\" , \n").arg(this->url.toString());
	json += QString("  \"description\": \"%1\" \n").arg(this->description);
	json = QString("{\n%1}").arg(json);
	return json;
}
