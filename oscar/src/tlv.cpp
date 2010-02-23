/****************************************************************************
 *  tlv.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/

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
