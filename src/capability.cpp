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

Capability::Capability()
{
}

Capability::Capability(const QByteArray &data)
{
	Q_ASSERT(data.size() == 16);
	m_data = data;
}

Capability::Capability(qint32 d1, qint32 d2, qint32 d3, qint32 d4)
{
	m_data += QByteArray::fromRawData(reinterpret_cast<const char *>(&d1), 4) +
			QByteArray::fromRawData(reinterpret_cast<const char *>(&d2), 4) +
			QByteArray::fromRawData(reinterpret_cast<const char *>(&d3), 4) +
			QByteArray::fromRawData(reinterpret_cast<const char *>(&d4), 4);
	Q_ASSERT(m_data.size() == 16);
}

bool Capability::operator==(const Capability &rhs) const
{
	return m_data == rhs.m_data;
}
