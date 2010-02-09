/****************************************************************************
 *  flap.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "flap.h"
#include "util.h"
#include <QIODevice>

namespace Icq
{

FLAP::FLAP(quint8 channel)
{
	m_channel = channel;
	m_state = ReadHeader;
}

QByteArray FLAP::toByteArray() const
{
	QByteArray data = header();
	data += m_data;
	return data;
}

QByteArray FLAP::header() const
{
	DataUnit data;
	data.append<quint8>(0x2a);
	data.append<quint8>(m_channel);
	data.append<quint16>(m_sequence_number);
	data.append<quint16>(m_data.size());
	return data;
}

bool FLAP::readData(QIODevice *dev)
{
	if (m_state == ReadHeader) {
		DataUnit data(dev->read(6));
		if (data.read<quint8>() != 0x2a)
			return false;
		m_channel = data.read<quint8>();
		m_sequence_number = data.read<quint16>();
		m_length = data.read<quint16>();
		m_state = ReadData;
		m_data.resize(m_length);
	}
	if (m_state == ReadData) {
		char *data = m_data.data() + m_data.size() - m_length;
		int readed = dev->read(data, m_length);
		if (readed < 0)
			return false;
		m_length -= readed;
	}
	if (m_length == 0)
		m_state = Finished;
	return true;
}

void FLAP::clear()
{
	m_state = ReadHeader;
	m_data.clear();
}

} // namespace Icq
