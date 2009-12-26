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

namespace Icq {

Capability::Capability()
{
}

Capability::Capability(const QByteArray &data)
{
	if (data.size() == 2) {
		data1 = shortUuid().data1;
		data1 |= qFromBigEndian<quint16>((const uchar *)data.constData());
		data2 = shortUuid().data2;
		data3 = shortUuid().data3;
		memcpy(data4, shortUuid().data4, 8);
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
	data2 = d2 >> 16;
	data3 = (d2 & 0x0000ffff);
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
	data2 = qFromBigEndian<quint16>(data + 4);
	data3 = qFromBigEndian<quint16>(data + 6);
	for (int i = 8; i < 16; i++)
		data4[i - 8] = data[i];
}


Capability::Capability(quint16 data): QUuid(shortUuid())
{
	data1 |= data;
}

bool Capability::isShort() const
{
	if((data1 >> 16) != 0x0946)
		return false;
	return data2 == shortUuid().data2
			&& data3 == shortUuid().data3
			&& !memcmp(data4, shortUuid().data4, 16);
}

bool Capability::operator==(const QUuid &rhs) const
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

template<typename T>
inline bool matchValue(T a, T b, int len)
{ return !memcmp(&a, &b, len); }

bool Capability::match(const Capability &c, quint8 len) const
{
	bool isS = isShort();
	bool cisS = c.isShort();
	if (isS != cisS) {
		return false;
	} else if (isS) {
		return data1 == c.data1;
	} else if (len > 16) {
		len = 16;
		while (len > 8 && !c.data4[len - 9])
			len--;
		if (len == 8) {
			if (!c.data3) {
				len -= 2;
				if (!c.data2) {
					len -= 2;
				} else if (!(c.data2 & 0xff))
					len --;
			}
			else if (!(c.data3 & 0xff))
				len--;
		}
		if (len == 4) {
			uint a = c.data1;
			while (a && !(a & 0xff)) {
				a >>= 8;
				len--;
			}
		}
	}
	if (len < 4) {
		return matchValue(qToBigEndian(data1), qToBigEndian(c.data1), len);
	} else if (len < 6) {
		return data1 == c.data1
				&& matchValue(qToBigEndian(data2), qToBigEndian(c.data2), len - 4);
	} else if (len < 8) {
		return data1 == c.data1
				&& data2 == c.data2
				&& matchValue(qToBigEndian(data3), qToBigEndian(c.data3), len - 6);
	} else {
		return data1 == c.data1
				&& data2 == c.data2
				&& data3 == c.data3
				&& !memcmp(data4, c.data4, len - 8);
	}
}

const QUuid &Capability::shortUuid()
{
	static const QUuid uuid("{09460000-4C7F-11D1-8222-444553540000}");
	return uuid;
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
