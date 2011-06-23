/****************************************************************************
 *  capability.h
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

#ifndef CAPABILTY_H
#define CAPABILTY_H

#include <QByteArray>
#include <QList>
#include <QHash>
#include <QUuid>
#include "dataunit.h"

namespace qutim_sdk_0_3 {

namespace oscar {

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
protected:
	static const QUuid &shortUuid();
};

class LIBOSCAR_EXPORT Capabilities: public QList<Capability>
{
public:
	bool match(const Capability &capability, quint8 len = Capability::Size) const;
	const_iterator find(const Capability &capability, quint8 len = Capability::Size) const;
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

inline uint qHash(const Capability &capability)
{
	return qHash(capability.data());
}

template<>
struct fromDataUnitHelper<Capability, false>
{
	static inline Capability fromByteArray(const DataUnit &d)
	{
		return Capability(d.readData(16));
	}
};

} } // namespace qutim_sdk_0_3::oscar

#endif // CAPABILTY_H
