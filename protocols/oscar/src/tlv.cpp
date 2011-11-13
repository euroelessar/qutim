/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "tlv.h"
#include <QDataStream>

namespace qutim_sdk_0_3 {

namespace oscar {

quint32 TLVMap::valuesSize() const
{
	quint32 size = 0;
	foreach(const TLV &tlv, *this)
		size = size + tlv.data().size() + 4;
	return size;
}

TLVMap::operator QByteArray() const
{
	QByteArray data;
	foreach(const TLV &tlv, *this)
		data += tlv;
	return data;
}

} } // namespace qutim_sdk_0_3::oscar

