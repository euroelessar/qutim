/****************************************************************************
 *  dataunit.cpp
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

#include "dataunit.h"

namespace Icq
{

TLV DataUnit::readTLV(ByteOrder bo) const
{
	TLV tlv(0xffff);
	if (dataSize() < 4)
		return tlv;
	quint16 len;
	if (bo == BigEndian) {
		tlv.setType(qFromBigEndian<quint16> ((const uchar *) m_data.constData() + m_state));
		len = qFromBigEndian<quint16> ((const uchar *) m_data.constData() + m_state + 2);
	} else {
		tlv.setType(qFromLittleEndian<quint16> ((const uchar *) m_data.constData() + m_state));
		len = qFromLittleEndian<quint16> ((const uchar *) m_data.constData() + m_state + 2);
	}
	m_state += 4;
	if (dataSize() < len) {
		tlv.setType(0xffff);
		m_state = m_data.size();
	} else {
		tlv.setValue(QByteArray::fromRawData(m_data.constData() + m_state, len));
		m_state += len;
	}
	return tlv;
}

} // namespace Icq
