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

#include "flap.h"
#include "util.h"
#include <QIODevice>

namespace qutim_sdk_0_3 {

namespace oscar {

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
		m_data += dev->read(6 - m_data.size());
		if (m_data.size() < 6)
			return true;
		DataUnit data(m_data);
		int dataSize = data.dataSize();
		quint8 checkValue = data.read<quint8>();
		if (checkValue != 0x2a) {
			qDebug() << "data.size() ==" << dataSize << "but 6 was expected";
			qDebug() << "dev->read() returned" << checkValue << ", but 0x2a was expected";
			return false;
		}
		m_channel = data.read<quint8>();
		m_sequence_number = data.read<quint16>();
		m_length = data.read<quint16>();
		m_state = ReadData;
		m_data.resize(m_length);
	}
	if (m_state == ReadData) {
		char *data = m_data.data() + m_data.size() - m_length;
		int readed = dev->read(data, m_length);
		if (readed < 0) {
			qDebug() << "dev->read() read" << readed << " bytes";
			return false;
		}
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

} } // namespace qutim_sdk_0_3::oscar

