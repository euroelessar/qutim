/****************************************************************************
 *  tlv.h
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

#ifndef TLV_H
#define TLV_H

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QtEndian>

#include "icq_global.h"
#include "capability.h"
#include "util.h"

class QDataStream;

namespace Icq {

class TLV;
class TLVMap;

class TLV
{
public:
	enum ByteOrder {
		BigEndian = QSysInfo::BigEndian,
		LittleEndian = QSysInfo::LittleEndian
	};

	TLV(quint16 type = 0x0000);
	inline const QByteArray &value() const { return m_value; }
	template<typename T>
	T value() const;
	template<typename T>
	void setValue(const T &value);
	template<typename T>
	void appendValue(const T &value);
	inline quint16 type() const { return m_type; }
	inline void setType(quint16 type) { m_type = type; }
	QByteArray toByteArray(ByteOrder bo = BigEndian) const;
	inline operator QByteArray() const { return toByteArray(BigEndian); }
	static inline TLV fromByteArray(const QByteArray &data, ByteOrder bo = BigEndian)
	{ return fromByteArray(data.constData(), data.size(), bo); }
	static TLV fromByteArray(const char *data, int length, ByteOrder bo = BigEndian);
	static TLVMap parseByteArray(const char *data, int length, ByteOrder bo = BigEndian);
	static inline TLVMap parseByteArray(const QByteArray &data, ByteOrder bo = BigEndian);
	template<typename T>
	static TLV fromTypeValue(quint16 type, T value);
private:
	inline void ensure_value() { if(m_value.size() > 0xffff) m_value.truncate(0xffff); } // 16 bits
	quint16 m_type;
	QByteArray m_value;
};

class TLVMap : public QMultiMap<quint16, TLV>
{
public:
	enum ByteOrder {
		BigEndian = QSysInfo::BigEndian,
		LittleEndian = QSysInfo::LittleEndian
	};
	inline TLVMap() {}
	inline TLVMap(const QMap<quint16, TLV> &other) : QMultiMap<quint16, TLV>(other) {}

//	TLVMap(const QByteArray &data, ByteOrder bo = BigEndian);

	inline TLV value(int key)
	{ return QMultiMap<quint16, TLV>::value(key); }
	inline TLV value(int key) const
	{ return QMultiMap<quint16, TLV>::value(key); }

	template<typename T>
	T value(quint16 type, const T &def = T()) const
	{
		TLVMap::const_iterator it = find(type);
		return it == constEnd() ? def : it->value<T>();
	}

	TLVMap::iterator insert(quint16 /*type*/, const TLV &/*data*/)
	{
		Q_ASSERT(0);
		return end();
	}

	template<typename T>
	TLVMap::iterator insert(quint16 type, const T &data)
	{
		return QMultiMap<quint16, TLV>::insert(type, TLV::fromTypeValue(type, data));
	}

	TLVMap::iterator insert(quint16 type)
	{
		return QMultiMap<quint16, TLV>::insert(type, TLV(type));
	}

	TLVMap::iterator insert(const TLV &tlv)
	{
		return QMultiMap<quint16, TLV>::insert(tlv.type(), tlv);
	}

	quint32 valuesSize() const
	{
		quint32 size = 0;
		foreach(const TLV &tlv, *this)
			size = size + tlv.value().size() + 4;
		return size;
	}

	operator QByteArray()  const
	{
		QByteArray data;
		foreach(const TLV &tlv, *this)
			data += tlv;
		return data;
	}
};

TLVMap TLV::parseByteArray(const QByteArray &data, ByteOrder bo)
{
	return parseByteArray(data.constData(), data.size(), bo);
}

template<>
Q_INLINE_TEMPLATE QByteArray TLV::value<QByteArray>() const
{
	return m_value;
}

template<>
Q_INLINE_TEMPLATE Capability TLV::value<Capability>() const
{
       return Capability(m_value);
}

template<>
Q_INLINE_TEMPLATE QString TLV::value<QString>() const
{
	return Util::defaultCodec()->toUnicode(m_value);
}

template<>
Q_INLINE_TEMPLATE quint8 TLV::value<quint8>() const
{
	return !m_value.isEmpty() ? m_value[0] : 0;
}

template<>
Q_INLINE_TEMPLATE qint8 TLV::value<qint8>() const
{
	return !m_value.isEmpty() ? m_value[0] : 0;
}

template<typename T>
Q_INLINE_TEMPLATE T TLV::value() const
{
	return uint(m_value.size()) >= sizeof(T) ? qFromBigEndian<T>((const uchar *)m_value.constData()) : T();
}

template<>
Q_INLINE_TEMPLATE void TLV::appendValue<QString>(const QString &value)
{
	m_value += Util::defaultCodec()->fromUnicode(value);
	ensure_value();
}

template<>
Q_INLINE_TEMPLATE void TLV::appendValue<QByteArray>(const QByteArray &value)
{
	m_value += value;
	ensure_value();
}

template<>
Q_INLINE_TEMPLATE void TLV::appendValue<Capability>(const Capability &value)
{
	m_value += value.data();
	ensure_value();
}

template<typename T>
Q_INLINE_TEMPLATE void TLV::appendValue(const T &value)
{
	char str[sizeof(T)];
	qToBigEndian<T>(value, (uchar *)str);
	m_value += QByteArray::fromRawData(str, sizeof(T));
}

template<typename T>
Q_INLINE_TEMPLATE void TLV::setValue(const T &value)
{
	m_value.clear();
	appendValue<T>(value);
}

template<typename T>
Q_INLINE_TEMPLATE TLV TLV::fromTypeValue(quint16 type, T value)
{
	TLV tlv(type);
	tlv.setValue(value);
	return tlv;
}

} // namespace Icq

#endif // TLV_H
