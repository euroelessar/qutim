/****************************************************************************
 *  capability.h
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

#ifndef CAPABILTY_H
#define CAPABILTY_H

#include <QByteArray>
#include <QList>
#include <QHash>
#include <QUuid>

namespace Icq {

struct Capability : public QUuid
{
public:
	Capability();
	Capability(const QByteArray &data);
	Capability(quint32 d1, quint32 d2, quint32 d3, quint32 d4);
	Capability(uint l, ushort w1, ushort w2, uchar b1, uchar b2,
			   uchar b3, uchar b4, uchar b5, uchar b6, uchar b7, uchar b8);
	Capability(quint8 d1, quint8 d2, quint8 d3, quint8 d4, quint8 d5, quint8 d6,
			   quint8 d7, quint8 d8, quint8 d9, quint8 d10, quint8 d11, quint8 d12,
			   quint8 d13, quint8 d14, quint8 d15, quint8 d16);
	Capability(quint16 data);
	QByteArray data() const;
	bool isShort() const;
	bool isEmpty() const { return isNull(); }
	bool operator==(const Capability &rhs) const;
	bool match(const Capability &capability, quint8 len = 17) const;
};

class Capabilities: public QList<Capability>
{
public:
	bool match(const Capability &capability, quint8 len = 17) const;
	const_iterator find(const Capability &capability, quint8 len = 17) const;
};

inline uint qHash(const Capability &capability)
{
	return qHash(capability.data());
}

} // namespace Icq

#endif // CAPABILTY_H
