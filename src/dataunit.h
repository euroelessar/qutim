/****************************************************************************
 *  dataunit.h
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

#ifndef DATAUNIT_H
#define DATAUNIT_H

#include "util.h"
#include "tlv.h"
#include "capability.h"

namespace Icq
{

enum ByteOrder
{
	BigEndian = QSysInfo::BigEndian,
	LittleEndian = QSysInfo::LittleEndian
};

class DataUnit
{
public:
	inline DataUnit() { m_state = 0; }
	inline DataUnit(const DataUnit &unit) { m_data = unit.m_data; m_state = 0; }
	inline DataUnit(const TLV &tlv) { m_data = tlv.value(); m_state = 0; }
	inline DataUnit(const QByteArray &data) { m_data = data; m_state = 0; }
	inline const QByteArray &data() const { return m_data; }
	inline void setData(const QByteArray &data) { m_data = data; }
	inline void appendData(const QByteArray &data) { m_data += data; }
	inline void appendData(const Capability &data) { m_data += data.data(); }
	inline void appendData(const QString &str, QTextCodec *codec = Util::defaultCodec()) { m_data += codec->fromUnicode(str); }
	void appendTLV(quint16 type) { appendData(TLV(type)); }
	template<typename T>
	void appendTLV(quint16 type, const T &value);
	template<typename T>
	void appendSimple(T value, ByteOrder bo = BigEndian);
	template<typename L>
	void appendData(const QByteArray &str, ByteOrder bo = BigEndian);
	template<typename L>
	void appendData(const QString &str, QTextCodec *codec = Util::defaultCodec(), ByteOrder bo = BigEndian);
	template<typename L>
	void appendData(const QString &str, ByteOrder bo);
	inline void resetState() const { m_state = 0; }
	inline uint dataSize() const { return m_data.size() > m_state ? m_data.size() - m_state : 0; }
	template<typename T>
	T readSimple(ByteOrder bo = BigEndian) const;
	template<typename L>
	QByteArray readData(ByteOrder bo = BigEndian) const;
	inline Capability readCapability() const;
	inline QByteArray readData(uint size) const;
	template<typename L>
	QString readString(QTextCodec *codec = Util::defaultCodec(), ByteOrder bo = BigEndian) const;
	template<typename L>
	QString readString(ByteOrder bo) const;
	inline void skipData(uint num) const { m_state = qMin<uint> (m_state + num, m_data.size()); }
	inline QByteArray readAll() const;
	TLV readTLV(ByteOrder bo = BigEndian) const;
	template<typename L>
	TLVMap readTLVChain(ByteOrder bo = BigEndian) const;
	inline TLVMap readTLVChain(ByteOrder bo = BigEndian) const;
protected:
	QByteArray m_data;
private:
	inline void appendSimple_helper(const QString &value, ByteOrder b) { appendSimple_helper(value.toUtf8(), b); }
	inline void appendSimple_helper(const QByteArray &value, ByteOrder b) { appendSimple_helper(value.data(), value.size(), b); }
	inline void appendSimple_helper(const char *value, ByteOrder b) { appendSimple_helper(value, qstrlen(value), b); }
	inline void appendSimple_helper(const char *data, int size, ByteOrder);
	template<typename T>
	void appendSimple_helper(T value, ByteOrder bo = BigEndian);
	mutable int m_state;
};

template<typename T>
Q_INLINE_TEMPLATE void DataUnit::appendTLV(quint16 type, const T &value)
{
	appendData(TLV::fromTypeValue(type, value));
}

template<>
Q_INLINE_TEMPLATE void DataUnit::appendTLV(quint16 type, const DataUnit &value)
{
	appendData(TLV::fromTypeValue(type, value.data()));
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::appendData(const QByteArray &str, ByteOrder bo)
{
	appendSimple<L> (str.size(), bo);
	m_data.append(str);
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::appendData(const QString &str, QTextCodec *codec, ByteOrder bo)
{
	appendData<L> (codec->fromUnicode(str), bo);
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::appendData(const QString &str, ByteOrder bo)
{
	appendData<L> (Util::defaultCodec()->fromUnicode(str), bo);
}

template<typename T>
Q_INLINE_TEMPLATE void DataUnit::appendSimple(T value, ByteOrder bo)
{
	appendSimple_helper(value, bo);
}

template<typename T>
Q_INLINE_TEMPLATE void DataUnit::appendSimple_helper(T value, ByteOrder bo)
{
	appendData(bo == BigEndian ? Util::toBigEndian(value) : Util::toLittleEndian(value));
}

void DataUnit::appendSimple_helper(const char *data, int size, ByteOrder)
{
	m_data += QByteArray::fromRawData(data, size);
}

template<>
Q_INLINE_TEMPLATE qint8 DataUnit::readSimple<qint8>(ByteOrder) const
{
	if (dataSize() < 1) {
		m_state = m_data.size();
		return 0;
	}
	return m_data.at(m_state++);
}

template<>
Q_INLINE_TEMPLATE quint8 DataUnit::readSimple<quint8>(ByteOrder bo) const
{
	return (quint8) readSimple<qint8> (bo);
}

template<typename T>
Q_INLINE_TEMPLATE T DataUnit::readSimple(ByteOrder bo) const
{
	if (dataSize() < sizeof(T)) {
		m_state = m_data.size();
		return 0;
	}
	m_state += sizeof(T);
	return bo == BigEndian ? qFromBigEndian<T> ((const uchar *) m_data.constData() + m_state - sizeof(T)) : qFromLittleEndian<T> ((const uchar *) m_data.constData() + m_state - sizeof(T));
}

template<typename L>
Q_INLINE_TEMPLATE QByteArray DataUnit::readData(ByteOrder bo) const
{
	return readData(readSimple<L> (bo));
}

Capability DataUnit::readCapability() const
{
	return Capability(readData(16));
}

QByteArray DataUnit::readData(uint size) const
{
	QByteArray str;
	if (dataSize() < size) {
		m_state = m_data.size();
		return str;
	}
	str = QByteArray::fromRawData(m_data.constData() + m_state, size);
	m_state += size;
	return str;
}

template<typename L>
Q_INLINE_TEMPLATE QString DataUnit::readString(QTextCodec *codec, ByteOrder bo) const
{
	return codec->toUnicode(readData<L> (bo));
}

template<typename L>
Q_INLINE_TEMPLATE QString DataUnit::readString(ByteOrder bo) const
{
	return Util::defaultCodec()->toUnicode(readData<L> (bo));
}

QByteArray DataUnit::readAll() const
{
	QByteArray data = QByteArray::fromRawData(m_data.constData() + m_state, dataSize());
	m_state = m_data.size();
	return data;
}

template<typename L>
Q_INLINE_TEMPLATE TLVMap DataUnit::readTLVChain(ByteOrder bo) const
{
	TLVMap tlvs;
	L count = readSimple<L> ();
	for (L i = 0; i < count; i++) {
		TLV tlv = readTLV(bo);
		if (tlv.type() == 0xffff)
			return tlvs;
		tlvs.insert(tlv);
	}
	return tlvs;
}

TLVMap DataUnit::readTLVChain(ByteOrder bo) const
{
	TLVMap tlvs;
	forever {
		TLV tlv = readTLV(bo);
		if (tlv.type() == 0xffff)
			return tlvs;
		tlvs.insert(tlv);
	}
	return tlvs;
}

} // namespace Icq

#endif // DATAUNIT_H
