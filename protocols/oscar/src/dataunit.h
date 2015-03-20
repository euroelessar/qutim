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

#ifndef DATAUNIT_H
#define DATAUNIT_H

#include "util.h"
#include "icq_global.h"
#include <typeinfo>
#include <limits>

namespace qutim_sdk_0_3 {

namespace oscar {

enum ByteOrder
{
	BigEndian = QSysInfo::BigEndian,
	LittleEndian = QSysInfo::LittleEndian
};

class TLV;
class TLVMap;

class LIBOSCAR_EXPORT DataUnit
{
public:
	DataUnit() { m_state = 0; m_max_size = 0; }
	DataUnit(const DataUnit &unit) { m_data = unit.m_data; m_state = 0; m_max_size = 0; }
	DataUnit(const QByteArray &data) { m_data = data; m_state = 0; m_max_size = 0; }
	const QByteArray &data() const { return m_data; }
	operator QByteArray() const { return data(); }
	void setData(const QByteArray &data) { m_data = data; m_state = 0; }
	inline QByteArray readData(uint size) const;
	inline void skipData(uint num) const { m_state = qMin<uint>(m_state + num, m_data.size()); }
	inline void resetState() const { m_state = 0; }
	inline uint dataSize() const { return m_data.size() > m_state ? m_data.size() - m_state : 0; }
	inline QByteArray readAll() const;
	int state() const { return m_state; }
	void setMaxSize(int size) { m_max_size = size; }
	bool canAppend(int size) { return m_data.size() + size <= m_max_size; }
	template<typename T>
	void append(const T& data);
	void append(const char *data);
	template<typename T>
	void append(const T &data, ByteOrder bo);
	void append(const QString &data, QTextCodec *codec);
	void append(const char *data, QTextCodec *codec);
	template<typename L>
	void append(const QByteArray &data, ByteOrder bo = BigEndian);
	template<typename L>
	void append(const QString &data, QTextCodec *codec, ByteOrder bo = BigEndian);
	template<typename L>
	void append(const char *data, QTextCodec *codec, ByteOrder bo = BigEndian);
	template<typename L>
	void append(const QString &data, ByteOrder bo = BigEndian);
	template<typename L>
	void append(const char *data, ByteOrder bo = BigEndian);
	template<typename T>
	T read() const;
	template<typename T>
	T read(ByteOrder bo) const;
	template<typename T, typename L>
	T read(ByteOrder bo = BigEndian, int = 0) const;
	template<typename T>
	T read(QTextCodec *codec) const;
	template<typename T, typename L>
	T read(QTextCodec *codec, ByteOrder bo = BigEndian) const;
	inline void appendTLV(quint16 type, ByteOrder bo = BigEndian);
	template<typename T>
	void appendTLV(quint16 type, const T &value, ByteOrder bo = BigEndian);
protected:
	QByteArray m_data;
	inline void ensure_value() { if (m_max_size > 0 && m_data.size() > m_max_size) m_data.truncate(m_max_size); } // 16 bits
private:
	int m_max_size;
	mutable int m_state;
};

QByteArray DataUnit::readData(uint size) const
{
	QByteArray str;
	size = qMin(dataSize(), size);
	str = m_data.mid(m_state, size);
	m_state += size;
	return str;
}


QByteArray DataUnit::readAll() const
{
	QByteArray data = QByteArray::fromRawData(m_data.constData() + m_state, dataSize());
	m_state = m_data.size();
	return data;
}

template<typename T>
struct is_simple
{
	static const bool value = false;
};

#define ADD_SIMPLE_TYPE(T)\
	template<> \
	struct is_simple<T> \
{ \
	static const bool value = true; \
}

ADD_SIMPLE_TYPE(quint8);
ADD_SIMPLE_TYPE(quint16);
ADD_SIMPLE_TYPE(quint32);
ADD_SIMPLE_TYPE(quint64);
ADD_SIMPLE_TYPE(qint8);
ADD_SIMPLE_TYPE(qint16);
ADD_SIMPLE_TYPE(qint32);
ADD_SIMPLE_TYPE(qint64);

template<typename T, bool is_int = is_simple<T>::value >
struct toDataUnitHelper;

template<typename T>
struct toDataUnitHelper<T, false>
{
	static inline QByteArray toByteArray(const T &data, ByteOrder)
	{
		return static_cast<QByteArray>(data);
	}
	static inline QByteArray toByteArray(const T &data)
	{
		return static_cast<QByteArray>(data);
	}
};

template<>
struct toDataUnitHelper<QString>
{
	static inline QByteArray toByteArray(const QString &data, QTextCodec *codec = Util::defaultCodec())
	{
		return codec->fromUnicode(data);
	}
	static inline QByteArray toByteArray(const QString &data, ByteOrder)
	{
		return toByteArray(data);
	}
};

template<typename T>
struct toDataUnitHelper<T, true>
{
	static inline QByteArray toByteArray(T data, ByteOrder bo = BigEndian)
	{
		return bo == BigEndian ? Util::toBigEndian(data) : Util::toLittleEndian(data);
	}
	static inline QByteArray toByteArray(const QByteArray &data, ByteOrder bo = BigEndian)
	{
		return toByteArray(data.size(), bo) + data;
	}
	static inline QByteArray toByteArray(const QString &data, ByteOrder bo = BigEndian)
	{
		return toByteArray(data, Util::defaultCodec(), bo);
	}
	static inline QByteArray toByteArray(const char *data, ByteOrder bo = BigEndian)
	{
		return toByteArray(QString(data), Util::defaultCodec(), bo);
	}
	static inline QByteArray toByteArray(const QString &data, QTextCodec *codec = Util::defaultCodec(), ByteOrder bo = BigEndian)
	{
		QByteArray buf = toDataUnitHelper<QString>::toByteArray(data, codec);
        if (static_cast<int>(std::numeric_limits<T>().max()) < static_cast<int>(buf.size()))
            buf.resize(std::numeric_limits<int>().max() - 1);
		return toByteArray(buf.size(), bo) + buf;
	}
	static inline QByteArray toByteArray(const char *data, QTextCodec *codec = Util::defaultCodec(), ByteOrder bo = BigEndian)
	{
		return toByteArray(QString(data), codec, bo);
	}
};

template<typename T>
Q_INLINE_TEMPLATE void DataUnit::append(const T& data)
{
	m_data += toDataUnitHelper<T>::toByteArray(data);
	ensure_value();
}

inline void DataUnit::append(const char *data)
{
	m_data += toDataUnitHelper<QString>::toByteArray(data);
	ensure_value();
}

template<typename T>
Q_INLINE_TEMPLATE void DataUnit::append(const T &data, ByteOrder bo)
{
	m_data += toDataUnitHelper<T>::toByteArray(data, bo);
	ensure_value();
}

Q_INLINE_TEMPLATE void DataUnit::append(const QString &data, QTextCodec *codec)
{
	m_data += toDataUnitHelper<QString>::toByteArray(data, codec);
	ensure_value();
}

inline void DataUnit::append(const char *data, QTextCodec *codec)
{
	m_data += toDataUnitHelper<QString>::toByteArray(data, codec);
	ensure_value();
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::append(const QByteArray &data, ByteOrder bo)
{
	m_data += toDataUnitHelper<L>::toByteArray(data, bo);
	ensure_value();
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::append(const QString &data, QTextCodec *codec, ByteOrder bo)
{
	m_data += toDataUnitHelper<L>::toByteArray(data, codec, bo);
	ensure_value();
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::append(const char *data, QTextCodec *codec, ByteOrder bo)
{
	m_data += toDataUnitHelper<L>::toByteArray(data, codec, bo);
	ensure_value();
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::append(const QString &data, ByteOrder bo)
{
	m_data += toDataUnitHelper<L>::toByteArray(data, bo);
	ensure_value();
}

template<typename L>
Q_INLINE_TEMPLATE void DataUnit::append(const char *data, ByteOrder bo)
{
	m_data += toDataUnitHelper<L>::toByteArray(data, bo);
	ensure_value();
}

template<typename T, bool is_int = is_simple<T>::value>
struct fromDataUnitHelper;

template<typename T>
struct fromDataUnitHelper<T, false>;

template<typename T>
struct fromDataUnitHelper<T, true>
{
	static inline T fromByteArray(const DataUnit &d, ByteOrder bo = BigEndian)
	{
		int state = d.state();
		d.skipData(sizeof(T));
		if (state >= d.data().size())
			return 0;
		return bo == BigEndian ?
					qFromBigEndian<T>((const uchar *) d.data().constData() + state) :
					qFromLittleEndian<T>((const uchar *) d.data().constData() + state);
	}
};

template<>
struct fromDataUnitHelper<qint8, true>
{
	static inline qint8 fromByteArray(const DataUnit &d, ByteOrder bo = BigEndian) // TODO: remove bo
	{
		Q_UNUSED(bo);
		if (d.dataSize() < 1)
			return 0;
		d.skipData(1);
		return d.data().at(d.state()-1);
	}
};

template<>
struct fromDataUnitHelper<quint8, true>
{
	static inline quint8 fromByteArray(const DataUnit &d, ByteOrder bo = BigEndian) // TODO: remove bo
	{
		Q_UNUSED(bo);
		return static_cast<quint8>(fromDataUnitHelper<qint8,true>::fromByteArray(d));
	}
};

template<>
struct fromDataUnitHelper<QString, false>
{
	template<class L>
	static inline QString fromByteArray(const DataUnit &d, QTextCodec *codec, L count)
	{
		return codec->toUnicode(d.readData(count));
	}
	template<class L>
	static inline QString fromByteArray(const DataUnit &d, L count, ByteOrder)
	{
		return fromByteArray<L>(d, Util::defaultCodec(), count);
	}
	static inline QString fromByteArray(const DataUnit &d, ByteOrder = BigEndian)
	{
		return Util::defaultCodec()->toUnicode(d.readAll());
	}
	static inline QString fromByteArray(const DataUnit &d, QTextCodec *codec)
	{
		return codec->toUnicode(d.readAll());
	}
};

template<>
struct fromDataUnitHelper<QByteArray, false>
{
	template<class L>
	static inline QByteArray fromByteArray(const DataUnit &d, L count, ByteOrder)
	{
		return d.readData(count);
	}

	static inline QByteArray fromByteArray(const DataUnit &d)
	{
		return d.readAll();
	}
};

template<>
struct fromDataUnitHelper<DataUnit, false>
{
	template<class L>
	static inline DataUnit fromByteArray(const DataUnit &d, L count, ByteOrder)
	{
		return DataUnit(d.readData(count));
	}

	static inline DataUnit fromByteArray(const DataUnit &d)
	{
		return DataUnit(d.readAll());
	}
};

template<typename T>
T DataUnit::read() const
{
	return fromDataUnitHelper<T>::fromByteArray(*this);
}

template<typename T>
T DataUnit::read(ByteOrder bo) const
{
	return fromDataUnitHelper<T>::fromByteArray(*this, bo);
}

template<typename T, typename L>
T DataUnit::read(ByteOrder bo, int) const
{
	return fromDataUnitHelper<T>::fromByteArray(*this, read<L>(bo), bo);
}

template<typename T>
T DataUnit::read(QTextCodec *codec) const
{
	return fromDataUnitHelper<T>::fromByteArray(*this, codec);
}

template<typename T, typename L>
T DataUnit::read(QTextCodec *codec, ByteOrder bo) const
{
	return fromDataUnitHelper<T>::fromByteArray(*this, codec, read<L>(bo));
}

} } // namespace qutim_sdk_0_3::oscar

#endif // DATAUNIT_H

