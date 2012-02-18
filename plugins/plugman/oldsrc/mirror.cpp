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

