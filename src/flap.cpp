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

namespace Icq {

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
	QByteArray data;
	data.resize(6);
	data[0] = 0x2a;
	data[1] = (uchar)m_channel;
	qToBigEndian<quint16>(m_sequence_number, (uchar *)data.constData() + 2);
	qToBigEndian<quint16>(m_data.size(), (uchar *)data.constData() + 4);
	return data;
}

bool FLAP::readData(QIODevice *dev)
{
	if(m_state == ReadHeader)
	{
		QByteArray data = dev->read(6);
		const uchar *s = (uchar *)data.constData();
		if(*(s++) != 0x2a)
			return false;
		m_channel = *(s++);
		m_sequence_number = qFromBigEndian<quint16>(s);
		m_length = qFromBigEndian<quint16>(s + 2);
		m_state = ReadData;
		m_data.resize(m_length);
	}
	if(m_state == ReadData)
	{
		char *data = m_data.data() + m_data.size() - m_length;
		int readed = dev->read(data, m_length);
		if(readed < 0)
			return false;
		m_length -= readed;
	}
	if(m_length == 0)
		m_state = Finished;
	return true;
}

void FLAP::clear()
{
	m_state = ReadHeader;
	m_data.clear();
}

} // namespace Icq
