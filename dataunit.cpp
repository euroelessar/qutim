#include "dataunit.h"

TLV DataUnit::readTLV(ByteOrder bo) const
{
	TLV tlv(0xffff);
	if(dataSize() < 4)
		return tlv;
	quint16 len;
	if(bo == BigEndian)
	{
		tlv.setType(qFromBigEndian<quint16>((const uchar *)m_data.constData() + m_state));
		len = qFromBigEndian<quint16>((const uchar *)m_data.constData() + m_state + 2);
	}
	else
	{
		tlv.setType(qFromLittleEndian<quint16>((const uchar *)m_data.constData() + m_state));
		len = qFromLittleEndian<quint16>((const uchar *)m_data.constData() + m_state + 2);
	}
	m_state += 4;
	if(dataSize() < len)
	{
		tlv.setType(0xffff);
		m_state = m_data.size();
	}
	else
	{
		tlv.setValue(QByteArray::fromRawData(m_data.constData() + m_state, len));
		m_state += len;
	}
	return tlv;
}
