/****************************************************************************
 *  tlv.h
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

#ifndef TLV_H
#define TLV_H

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QtEndian>
#include "icq_global.h"
#include "util.h"
#include "dataunit.h"

class QDataStream;

namespace qutim_sdk_0_3 {

namespace oscar {

class TLV;
class TLVMap;

class LIBOSCAR_EXPORT TLV: public DataUnit
{
public:
	inline TLV(quint16 type = 0x0000);
	template<typename T>
	inline TLV(quint16 type, const T &data);
	inline quint16 type() const { return m_type; }
	inline void setType(quint16 type) { m_type = type; }
	inline QByteArray toByteArray(ByteOrder bo = BigEndian) const;
	inline operator QByteArray() const { return toByteArray(BigEndian); }
	static inline TLV fromByteArray(const QByteArray &data, ByteOrder bo = BigEndian);
private:
	quint16 m_type;
};

class TLVMap: public QMap<quint16, TLV>
{
public:
	inline TLVMap();
	inline TLVMap(const QMap<quint16, TLV> &other);
	inline TLV value(int key) const;
	template<typename T>
	T value(quint16 type, const T &def = T()) const;
	template<typename T>
	TLVMap::iterator insert(quint16 type, const T &data);
	inline TLVMap::iterator insert(quint16 type);
	inline TLVMap::iterator insert(const TLV &tlv);
	quint32 valuesSize() const;
	operator QByteArray() const;
	inline static TLVMap fromByteArray(const QByteArray &data, ByteOrder bo = BigEndian);
private:
	TLVMap::iterator insert(quint16 type, const TLV &data);
};


TLV::TLV(quint16 type)
{
	m_type = type;
	setMaxSize(0xffff);
}

template<typename T>
Q_INLINE_TEMPLATE TLV::TLV(quint16 type, const T &data):
	m_type(type)
{
	append(data);
}

QByteArray TLV::toByteArray(ByteOrder bo) const
{
	DataUnit data;
	data.append<quint16>(m_type);
	data.append<quint16>(m_data);
	return data.data();
}

TLV TLV::fromByteArray(const QByteArray &data, ByteOrder bo)
{
   return DataUnit(data).read<TLV>(bo);
}

TLVMap::TLVMap()
{
}

TLVMap::TLVMap(const QMap<quint16, TLV> &other) :
	QMap<quint16, TLV>(other)
{
}

TLV TLVMap::value(int key) const
{
	return QMap<quint16, TLV>::value(key);
}

template<typename T>
Q_INLINE_TEMPLATE T TLVMap::value(quint16 type, const T &def) const
{
	TLVMap::const_iterator it = find(type);
	return it == constEnd() ? def : it->read<T>();
}

template<typename T>
Q_INLINE_TEMPLATE TLVMap::iterator TLVMap::insert(quint16 type, const T &data)
{
	return QMap<quint16, TLV>::insert(type, TLV(type, data));
}

TLVMap::iterator TLVMap::insert(quint16 type)
{
	return QMap<quint16, TLV>::insert(type, TLV(type));
}

TLVMap::iterator TLVMap::insert(const TLV &tlv)
{
	return QMap<quint16, TLV>::insert(tlv.type(), tlv);
}

TLVMap TLVMap::fromByteArray(const QByteArray &data, ByteOrder bo)
{
	return DataUnit(data).read<TLVMap>(bo);
}

void DataUnit::appendTLV(quint16 type)
{
	append(TLV(type));
}

template<typename T>
Q_INLINE_TEMPLATE void DataUnit::appendTLV(quint16 type, const T &data)
{
	append(TLV(type, data));
}

template<>
Q_INLINE_TEMPLATE void DataUnit::appendTLV(quint16 type, const DataUnit &data)
{
	append(TLV(type, data.data()));
}

template<>
struct fromDataUnitHelper<TLV>
{
	static inline TLV fromByteArray(const DataUnit &d, ByteOrder bo = BigEndian)
	{
		TLV tlv(0xffff);
		if (d.dataSize() < 4)
			return tlv;
		tlv.setType(d.read<quint16>(bo));
		if (d.dataSize() < 2)
			tlv.setType(0xffff);
		else
			tlv.append(d.read<QByteArray, quint16>(bo));
		return tlv;
	}
};

template<>
struct fromDataUnitHelper<TLVMap>
{
	static inline TLVMap fromByteArray(const DataUnit &d, ByteOrder bo = BigEndian)
	{
		TLVMap tlvs;
		forever {
			TLV tlv = fromDataUnitHelper<TLV>::fromByteArray(d, bo);
			if (tlv.type() == 0xffff)
				return tlvs;
			tlvs.insert(tlv);
		}
		return tlvs;
	}
	template<class L>
	static inline TLVMap fromByteArray(const DataUnit &d, L count, ByteOrder bo = BigEndian)
	{
		TLVMap tlvs;
		for (L i = 0; i < count; i++) {
			TLV tlv = fromDataUnitHelper<TLV>::fromByteArray(d, bo);
			if (tlv.type() == 0xffff)
				return tlvs;
			tlvs.insert(tlv);
		}
		return tlvs;
	}
};

} } // namespace qutim_sdk_0_3::oscar

#endif // TLV_H
