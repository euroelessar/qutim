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

#ifndef CAPABILTY_H
#define CAPABILTY_H

#include <QByteArray>
#include <QList>
#include <QHash>
#include <QUuid>
#include "dataunit.h"

class QDebug;

namespace qutim_sdk_0_3 {

namespace oscar {

class Capabilities;

struct LIBOSCAR_EXPORT Capability: public QUuid
{
public:
	enum { Size = 16 };
	Capability();
	Capability(const QString &str);
	Capability(const QByteArray &data);
	Capability(quint32 d1, quint32 d2, quint32 d3, quint32 d4);
	Capability(uint l, ushort w1, ushort w2, uchar b1, uchar b2, uchar b3,
			uchar b4, uchar b5, uchar b6, uchar b7, uchar b8);
	Capability(quint8 d1, quint8 d2, quint8 d3, quint8 d4, quint8 d5, quint8 d6,
			quint8 d7, quint8 d8, quint8 d9, quint8 d10, quint8 d11, quint8 d12,
			quint8 d13, quint8 d14, quint8 d15, quint8 d16);
	Capability(quint16 data);
	QByteArray data() const;
	operator QByteArray() const { return data(); }
	bool isShort() const;
	bool isEmpty() const { return isNull(); }
	bool operator==(const QUuid &rhs) const;
	bool match(const Capability &capability, quint8 len = Size) const;
	QString name() const;
	uint hash() const;
protected:
	quint8 nonZeroLength() const;
	static const QUuid &shortUuid();
	friend class Capabilities;
};

class LIBOSCAR_EXPORT Capabilities: public QList<Capability>
{
public:
	enum { UpToFirstZero = 0xff };
	bool match(const Capability &capability, quint8 len = UpToFirstZero) const;
	const_iterator find(const Capability &capability, quint8 len = UpToFirstZero) const;
};

class LIBOSCAR_EXPORT StandartCapability : public Capability
{
public:
	StandartCapability(const QString &name, const QString &str);
	StandartCapability(const QString &name, const QByteArray &data);
	StandartCapability(const QString &name, quint32 d1, quint32 d2, quint32 d3, quint32 d4);
	StandartCapability(const QString &name, uint l, ushort w1, ushort w2, uchar b1,
			   uchar b2, uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8);
	StandartCapability(const QString &name, quint8 d1, quint8 d2, quint8 d3, quint8 d4,
			   quint8 d5, quint8 d6, quint8 d7, quint8 d8, quint8 d9, quint8 d10,
			   quint8 d11, quint8 d12, quint8 d13, quint8 d14, quint8 d15, quint8 d16);
	StandartCapability(const QString &name, quint16 data);
};

template<>
struct fromDataUnitHelper<Capability, false>
{
	static inline Capability fromByteArray(const DataUnit &d)
	{
		return Capability(d.readData(16));
	}
};

inline uint Capability::hash() const
{
	const uint h1 = qHash((quint64(data1) << 32) | (quint64(data2) << 16) | (quint64(data3)));
	uint h2 = qHash(qFromLittleEndian<quint64>(reinterpret_cast<const uchar*>(data4)));
	return qHash((quint64(h1) << 32) | quint64(h2));
}

LIBOSCAR_EXPORT QDebug operator<<(QDebug debug, Capability &capability);

} } // namespace qutim_sdk_0_3::oscar

inline uint qHash(const qutim_sdk_0_3::oscar::Capability &capability)
{
	return capability.hash();
}

#endif // CAPABILTY_H

