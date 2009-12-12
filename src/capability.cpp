/****************************************************************************
 *  capability.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "capability.h"
#include <QDataStream>
#include <QtEndian>
#include <QDebug>

namespace Icq {

static const QUuid shortUuid("{09460000-4C7F-11D1-8222-444553540000}");

Capability::Capability()
{
}

Capability::Capability(const QByteArray &data)
{
	if (data.size() == 2) {
		data1 = shortUuid.data1;
		data1 |= qFromBigEndian<quint16>((const uchar *)data.constData());
		data2 = shortUuid.data2;
		data3 = shortUuid.data3;
		memcpy(data4, shortUuid.data4, 8);
	} else if (data.size() == 16) {
		const uchar *src = (const uchar *)data.constData();
		data1 = qFromBigEndian<quint32>(src);
		data2 = qFromBigEndian<quint16>(src + 4);
		data3 = qFromBigEndian<quint16>(src + 6);
		memcpy(data4, src + 8, 8);
	}
}

Capability::Capability(quint32 d1, quint32 d2, quint32 d3, quint32 d4)
{
	data1 = d1;
	data2 = (d2 & 0xff00) >> 8;
	data3 = (d2 & 0x00ff);
	qToBigEndian(d3, (uchar *)data4);
	qToBigEndian(d4, (uchar *)data4 + 4);
}

Capability::Capability(uint l, ushort w1, ushort w2, uchar b1, uchar b2,
					   uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8)
: QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)
{
}

Capability::Capability(quint8 d1, quint8 d2, quint8 d3, quint8 d4, quint8 d5, quint8 d6,
					   quint8 d7, quint8 d8, quint8 d9, quint8 d10, quint8 d11, quint8 d12,
					   quint8 d13, quint8 d14, quint8 d15, quint8 d16)
{
	uchar data[16] = { d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16 };
	data1 = qFromBigEndian<quint32>(data);
	data2 = qFromBigEndian<quint16>(data);
	data3 = qFromBigEndian<quint16>(data);
	for (int i = 8; i < 16; i++)
		data4[i - 8] = data[i];
}


Capability::Capability(quint16 data) : QUuid(shortUuid)
{
	data1 = ((data1 & 0xffff0000) >> 16) | data;
}

bool Capability::isShort() const
{
	if((data1 >> 16) != 0x0946)
		return false;
	return data2 == shortUuid.data2
			&& data3 == shortUuid.data3
			&& !memcmp(data4, shortUuid.data4, 16);
}

bool Capability::operator==(const Capability &rhs) const
{
	return QUuid::operator ==(rhs);
}

QByteArray Capability::data() const
{
	QByteArray data(16, Qt::Uninitialized);
	qToBigEndian(data1, (uchar*)data.data());
	qToBigEndian(data2, (uchar*)data.data() + 4);
	qToBigEndian(data3, (uchar*)data.data() + 6);
	memcpy(data.data() + 8, data4, 8);
	return data;
}

bool Capability::match(const Capability &c, quint8 len) const
{
	bool isS = isShort();
	bool cisS = c.isShort();
	if (isS != cisS)
		return false;
	if (isS)
		return data1 == c.data1;
	if (len > 16) {
		len = 16;
		while (len >= 8 && (!data4[len - 9] || !c.data4[len - 9]))
			len--;
	}
	if (data1 != c.data1
		|| data2 != c.data2
		|| data3 != c.data3)
		return false;
	return !memcmp(data4, c.data4, len - 8);
}

bool Capabilities::match(const Capability &capability, quint8 len) const
{
	foreach (const Capability &cap, *this) {
		if(cap.match(capability, len))
			return true;
	}
	return false;
}

Capabilities::const_iterator Capabilities::find(const Capability &capability, quint8 len) const
{
	const_iterator itr = constBegin();
	const_iterator end_itr = constEnd();
	while (itr != end_itr) {
		if (itr->match(capability, len))
			return itr;
		++itr;
	}
	return end_itr;
}

} // namespace Icq
