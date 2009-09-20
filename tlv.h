#ifndef TLV_H
#define TLV_H

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QtEndian>

#include "icq_global.h"

class QDataStream;
class TLV;

typedef QMap<quint16, TLV> TLVMap;

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
	static inline TLVMap parseByteArray(const QByteArray &data, ByteOrder bo = BigEndian)
	{ return parseByteArray(data.constData(), data.size(), bo); }
	static TLV fromTypeValue(quint16 type, const QByteArray &value);
	static TLV fromTypeValue(quint16 type, const QString &value);
	template<typename T>
	static TLV fromTypeValue(quint16 type, T value);
private:
	inline void ensure_value() { if(m_value.size() > 0xffff) m_value.truncate(0xffff); } // 16 bits
	quint16 m_type;
	QByteArray m_value;
};

template<>
Q_INLINE_TEMPLATE QByteArray TLV::value<QByteArray>() const
{
	return m_value;
}

template<>
Q_INLINE_TEMPLATE QString TLV::value<QString>() const
{
	return QString::fromUtf8(m_value);
}

template<typename T>
Q_INLINE_TEMPLATE T TLV::value() const
{
	return uint(m_value.size()) >= sizeof(T) ? qFromBigEndian<T>((const uchar *)m_value.constData()) : T();
}

template<>
Q_INLINE_TEMPLATE void TLV::appendValue<QString>(const QString &value)
{
	m_value += value.toUtf8();
	ensure_value();
}

template<>
Q_INLINE_TEMPLATE void TLV::appendValue<QByteArray>(const QByteArray &value)
{
	m_value += value;
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

inline TLV TLV::fromTypeValue(quint16 type, const QByteArray &value)
{
	TLV tlv(type);
	tlv.setValue(value);
	return tlv;
}

inline TLV TLV::fromTypeValue(quint16 type, const QString &value)
{
	TLV tlv(type);
	tlv.setValue(value);
	return tlv;
}

template<typename T>
Q_INLINE_TEMPLATE TLV TLV::fromTypeValue(quint16 type, T value)
{
	TLV tlv(type);
	tlv.setValue(value);
	return tlv;
}

#endif // TLV_H
