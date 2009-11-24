/****************************************************************************
 *  capability.cpp
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

#include "capability.h"
#include <QDataStream>
#include <QDebug>

namespace Icq {

Capability::Capability()
{
}

Capability::Capability(const QByteArray &data)
{
	Q_ASSERT(data.size() == 16 || data.size() == 2);
	m_data = data;
	m_is_short = (data.size() == 2);
	if(!m_is_short)
		stripLasts();
}

Capability::Capability(quint32 d1, quint32 d2, quint32 d3, quint32 d4)
{
	QDataStream stream(&m_data, QIODevice::WriteOnly);
	stream << d1 << d2 << d3 << d4;
	m_is_short = false;
	Q_ASSERT(m_data.size() == 16);
	stripLasts();
}

Capability::Capability(quint8 d1, quint8 d2, quint8 d3, quint8 d4, quint8 d5, quint8 d6,
					   quint8 d7, quint8 d8, quint8 d9, quint8 d10, quint8 d11, quint8 d12,
					   quint8 d13, quint8 d14, quint8 d15, quint8 d16)
{
	QDataStream stream(&m_data, QIODevice::WriteOnly);
	stream << d1 << d2 << d3 << d4 << d5 << d6 << d7 << d8 << d9 << d10 << d11 << d12 << d13 << d14 << d15 << d16;
	Q_ASSERT(m_data.size() == 16);
	stripLasts();
	qDebug() << m_data.toHex();
}


Capability::Capability(quint16 data)
{
	QDataStream stream(&m_data, QIODevice::WriteOnly);
	stream << data;
	m_is_short = true;
	Q_ASSERT(m_data.size() == 2);
}

bool Capability::operator==(const Capability &rhs) const
{
	return m_data == rhs.m_data;
}

void Capability::stripLasts()
{
	QByteArray::iterator itr = m_data.end();
	QByteArray::iterator itr_first = m_data.begin();
	int count = m_data.count();
	do
	{
		--itr;
		if(*itr != 0)
			break;
		--count;
	}
	while(itr != itr_first);
	m_data.truncate(count);
}

Capabilities::iterator Capabilities::match(const Capability &capability)
{
	iterator itr = begin();
	iterator end_itr = end();
	while(itr != end_itr)
	{
		if(itr->match(capability))
			return itr;
		++itr;
	}
	return end_itr;
}

} // namespace Icq
