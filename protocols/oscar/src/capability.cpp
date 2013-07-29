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

#include "capability.h"
#include <QDataStream>
#include <QtEndian>

namespace qutim_sdk_0_3 {

namespace oscar {

typedef QHash<Capability, QString> CapName;
Q_GLOBAL_STATIC(CapName, capName)

Capability::Capability()
{
}

Capability::Capability(const QString &str) :
	QUuid(str)
{
}

Capability::Capability(const QByteArray &data)
{
	if (data.size() == 2) {
		data1 = shortUuid().data1;
		data1 |= qFromBigEndian<quint16>((const uchar *) data.constData());
		data2 = shortUuid().data2;
		data3 = shortUuid().data3;
		memcpy(data4, shortUuid().data4, sizeof(data4));
	} else if (data.size() == 16) {
		const uchar *src = (const uchar *) data.constData();
		data1 = qFromBigEndian<quint32>(src);
		data2 = qFromBigEndian<quint16>(src + 4);
		data3 = qFromBigEndian<quint16>(src + 6);
		memcpy(data4, src + 8, sizeof(data4));
	}
}

Capability::Capability(quint32 d1, quint32 d2, quint32 d3, quint32 d4)
{
	data1 = d1;
	data2 = d2 >> 16;
	data3 = (d2 & 0x0000ffff);
	qToBigEndian(d3, (uchar *) data4);
	qToBigEndian(d4, (uchar *) data4 + 4);
}

Capability::Capability(uint l, ushort w1, ushort w2, uchar b1, uchar b2,
		uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8) :
	QUuid(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)
{
}

Capability::Capability(quint8 d1, quint8 d2, quint8 d3, quint8 d4, quint8 d5,
		quint8 d6, quint8 d7, quint8 d8, quint8 d9, quint8 d10, quint8 d11,
		quint8 d12, quint8 d13, quint8 d14, quint8 d15, quint8 d16)
{
	uchar data[16] =
	{ d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16 };
	data1 = qFromBigEndian<quint32>(data);
	data2 = qFromBigEndian<quint16>(data + 4);
	data3 = qFromBigEndian<quint16>(data + 6);
	for (int i = 8; i < 16; i++)
		data4[i - 8] = data[i];
}

Capability::Capability(quint16 data) :
	QUuid(shortUuid())
{
	data1 |= data;
}

bool Capability::isShort() const
{
	if ((data1 >> 16) != 0x0946)
		return false;
	return data2 == shortUuid().data2 && data3 == shortUuid().data3 && !memcmp(data4, shortUuid().data4, sizeof(data4));
}

bool Capability::operator==(const QUuid &rhs) const
{
	return QUuid::operator ==(rhs);
}

QByteArray Capability::data() const
{
	QByteArray data(16, Qt::Uninitialized);
	qToBigEndian(data1, (uchar*) data.data());
	qToBigEndian(data2, (uchar*) data.data() + 4);
	qToBigEndian(data3, (uchar*) data.data() + 6);
	memcpy(data.data() + 8, data4, 8);
	return data;
}

#define OSCAR_CONVERT_CAPABILTY(capability, buffer) \
	qToBigEndian((capability)->data1, buffer); \
	if (len > 4) { \
		qToBigEndian((capability)->data2, buffer + 4); \
		if (len > 6) { \
			qToBigEndian((capability)->data3, buffer + 6); \
			if (len > 8) { \
				memcpy(buffer + 8, (capability)->data4, len - 8); \
			} \
		} \
	}

bool Capability::match(const Capability &o, quint8 len) const
{
	len = qMin<quint8>(len, Size);
#if Q_BYTE_ORDER == Q_BIG_ENDIAN 
	if (sizeof(qutim_sdk_0_3::oscar::Capability) == 16)
		return !memcmp(this, &o, len);
#endif
	uchar a[Size];
	uchar b[Size];
	OSCAR_CONVERT_CAPABILTY(this, a);
	OSCAR_CONVERT_CAPABILTY(&o, b);
	return !memcmp(a, b, len);
}

QString Capability::name() const
{
	QString name = capName()->value(*this);
	if (name.isNull()) {
		if (!isShort()) {
			return toString();
		} else {
			return QString::number(data1 & 0xffff, 16);
		}
	}
	return name;
}

quint8 Capability::nonZeroLength() const
{
	uchar buf[Size];
	enum { len = Size };
	OSCAR_CONVERT_CAPABILTY(this, buf);
	int i;
	for (i = 15; i >= 0; --i) {
		if (buf[i])
			break;
	}
	return i + 1;
}

#undef OSCAR_CONVERT_CAPABILTY

const QUuid &Capability::shortUuid()
{
	static const QUuid uuid("{09460000-4C7F-11D1-8222-444553540000}");
	return uuid;
}

bool Capabilities::match(const Capability &capability, quint8 len) const
{
	return find(capability, len) != constEnd();
}

Capabilities::const_iterator Capabilities::find(const Capability &capability, quint8 len) const
{
	if (len == UpToFirstZero)
		len = capability.nonZeroLength();
	const_iterator itr = constBegin();
	const const_iterator end_itr = constEnd();
	for (; itr != end_itr; ++itr) {
		if (itr->match(capability, len))
			break;
	}
	return itr;
}

StandartCapability::StandartCapability(const QString &name, const QString &str) :
	Capability(str)
{
	capName()->insert(*this, name);
}

StandartCapability::StandartCapability(const QString &name, const QByteArray &data) :
	Capability(data)
{
	capName()->insert(*this, name);
}

StandartCapability::StandartCapability(const QString &name, quint32 d1, quint32 d2, quint32 d3, quint32 d4) :
	Capability(d1, d2, d3, d4)
{
	capName()->insert(*this, name);
}

StandartCapability::StandartCapability(const QString &name, uint l, ushort w1, ushort w2, uchar b1,
									   uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8) :
	Capability(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)
{
	capName()->insert(*this, name);
}

StandartCapability::StandartCapability(const QString &name, quint8 d1, quint8 d2, quint8 d3, quint8 d4,
		   quint8 d5, quint8 d6, quint8 d7, quint8 d8, quint8 d9, quint8 d10,
		   quint8 d11, quint8 d12, quint8 d13, quint8 d14, quint8 d15, quint8 d16) :

	Capability(d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16)
{
	capName()->insert(*this, name);
}

StandartCapability::StandartCapability(const QString &name, quint16 data) :
	Capability(data)
{
	capName()->insert(*this, name);
}

} } // namespace qutim_sdk_0_3::oscar

QDebug operator<<(QDebug debug, qutim_sdk_0_3::oscar::Capability &capability)
{
	debug.nospace() << capability.toString();
	return debug.space();
}
