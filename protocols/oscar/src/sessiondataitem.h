/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef SESSIONDATAITEM_H
#define SESSIONDATAITEM_H

#include "dataunit.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class LIBOSCAR_EXPORT SessionDataItem : public DataUnit
{
public:
	inline SessionDataItem(quint16 type = 0x0000, quint8 flags = 0x0000);
	inline SessionDataItem(const DataUnit &data);
	inline SessionDataItem(const SessionDataItem &item);
	inline quint16 type() const;
	inline void setType(quint16 type);
	inline quint8 flags() const;
	inline void setFlags(quint8 flags);
	inline operator QByteArray() const;
private:
	quint16 m_type;
	quint8 m_flags;
};

class LIBOSCAR_EXPORT SessionDataItemMap : public QMap<quint16, SessionDataItem>
{
public:
	inline SessionDataItemMap();
	inline SessionDataItemMap(const DataUnit &data);
	inline SessionDataItemMap(const QMap<quint16, SessionDataItem> &other);
	inline SessionDataItem value(int key) const;
	template<typename T>
	T value(quint16 type, const T &def = T()) const;
	inline SessionDataItemMap::iterator insert(quint16 type, quint8 flags);
	inline SessionDataItemMap::iterator insert(const SessionDataItem &item);
	inline operator QByteArray() const;
private:
	iterator insert(quint16 type, const TLV &data);
};

SessionDataItem::SessionDataItem(quint16 type, quint8 flags) :
	m_type(type), m_flags(flags)
{
	setMaxSize(253);
}

SessionDataItem::SessionDataItem(const DataUnit &data)
{
	m_type = data.read<quint16>();
	m_flags = data.read<quint8>();
	m_data = data.read<QByteArray, quint8>();
}

SessionDataItem::SessionDataItem(const SessionDataItem &item) :
	DataUnit(item), m_type(item.m_type), m_flags(item.m_flags)
{
}

quint16 SessionDataItem::type() const
{
	return m_type;
}

void SessionDataItem::setType(quint16 type)
{
	m_type = type;
}

inline quint8 SessionDataItem::flags() const
{
	return m_flags;
}

inline void SessionDataItem::setFlags(quint8 flags)
{
	m_flags = flags;
}

inline SessionDataItem::operator QByteArray() const
{
	DataUnit data;
	data.append<quint16>(m_type);
	data.append<quint8>(m_flags);
	data.append<quint8>(m_data);
	return data.data();
}

SessionDataItemMap::SessionDataItemMap() :
	QMap<quint16, SessionDataItem>()
{
}

SessionDataItemMap::SessionDataItemMap(const DataUnit &data)
{
	while (data.dataSize() >= 4)
		insert(SessionDataItem(data));
}

SessionDataItemMap::SessionDataItemMap(const QMap<quint16, SessionDataItem> &other):
	QMap<quint16, SessionDataItem>(other)
{
}

SessionDataItem SessionDataItemMap::value(int key) const
{
	return QMap<quint16, SessionDataItem>::value(key);
}

SessionDataItemMap::iterator SessionDataItemMap::insert(quint16 type, quint8 flags)
{
	return QMap<quint16, SessionDataItem>::insert(type, SessionDataItem(type, flags));
}

SessionDataItemMap::iterator SessionDataItemMap::insert(const SessionDataItem &item)
{
	return QMap<quint16, SessionDataItem>::insert(item.type(), item);
}

SessionDataItemMap::operator QByteArray() const
{
	DataUnit data;
	foreach (const SessionDataItem &item, *this)
		data.append(static_cast<QByteArray>(item));
	return data.data();
}


template<>
struct fromDataUnitHelper<SessionDataItem>
{
	static inline SessionDataItem fromByteArray(const DataUnit &d)
	{
		return SessionDataItem(d);
	}
};

template<>
struct fromDataUnitHelper<SessionDataItemMap>
{
	static inline SessionDataItemMap fromByteArray(const DataUnit &d)
	{
		return SessionDataItem(d);
	}
};

} } // namespace qutim_sdk_0_3::oscar

#endif // SESSIONDATAITEM_H

