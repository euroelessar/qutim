#ifndef DATAUNIT_H
#define DATAUNIT_H

#include "util.h"
#include "tlv.h"

class DataUnit
{
public:
	enum ByteOrder {
		BigEndian = QSysInfo::BigEndian,
		LittleEndian = QSysInfo::LittleEndian
	};
	inline DataUnit() { m_state = 0; }
	inline DataUnit(const DataUnit &unit) { m_data = unit.m_data; m_state = 0; }
	inline DataUnit(const TLV &tlv) { m_data = tlv.value(); m_state = 0; }
	inline DataUnit(const QByteArray &data) { m_data = data; m_state = 0; }
	inline const QByteArray &data() const { return m_data; }
	inline void setData(const QByteArray &data) { m_data = data; }
	inline void appendData(const QByteArray &data) { m_data += data; }
	template<typename T>
	void appendTLV(quint16 type, const T &value);
	template<typename T>
	void appendSimple(T value, ByteOrder bo = BigEndian);
	template<typename L>
	void appendData(const QByteArray &str);
	inline void resetState() { m_state = 0; }
	inline uint dataSize() const { return m_data.size() > m_state ? m_data.size() - m_state : 0; }
	template<typename T>
	T readSimple(ByteOrder bo = BigEndian) const;
	template<typename L>
	QByteArray readData() const;
	template<typename L>
	QString readString() const;
	inline void skipData(uint num) const { m_state = qMin<uint>(m_state + num, m_data.size()); }
	inline QByteArray readAll() const;
	TLV readTLV(ByteOrder bo = BigEndian) const;
	template<typename L>
	TLVMap readTLVChain(ByteOrder bo = BigEndian) const;
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

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::appendData(const QByteArray &str)
{
	m_data.append(Util::toBigEndian<L>(str.size()));
	m_data.append(str);
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
	if(dataSize() < 1)
	{
		m_state = m_data.size();
		return 0;
	}
	return m_data.at(m_state++);
}

template<>
Q_INLINE_TEMPLATE quint8 DataUnit::readSimple<quint8>(ByteOrder bo) const
{
	return (quint8)readSimple<qint8>(bo);
}

template<typename T>
Q_INLINE_TEMPLATE T DataUnit::readSimple(ByteOrder bo) const
{
	if(dataSize() < sizeof(T))
	{
		m_state = m_data.size();
		return 0;
	}
	m_state += sizeof(T);
	return bo == BigEndian
			? qFromBigEndian<T>((const uchar *)m_data.constData() + m_state - sizeof(T))
			: qFromLittleEndian<T>((const uchar *)m_data.constData() + m_state - sizeof(T));
}

template<typename L>
Q_INLINE_TEMPLATE QByteArray DataUnit::readData() const
{
	QByteArray str;
	L len = readSimple<L>();
	if(dataSize() < len)
	{
		m_state = m_data.size();
		return str;
	}
	str = QByteArray::fromRawData(m_data.constData() + m_state, len);
	m_state += len;
	return str;
}

template<typename L>
Q_INLINE_TEMPLATE QString DataUnit::readString() const
{
	return QString::fromUtf8(readData<L>());
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
	L count = readSimple<L>();
	for(L i = 0; i < count; i++)
	{
		TLV tlv = readTLV(bo);
		if(tlv.type() == 0xffff)
			return tlvs;
		tlvs.insert(tlv.type(), tlv);
	}
	return tlvs;
}

#endif // DATAUNIT_H
