#include "tlv.h"
#include <QDataStream>

TLV::TLV(quint16 type)
{
	m_type = type;
}

QByteArray TLV::toByteArray(ByteOrder bo) const
{
	QByteArray value;
	value.resize(4);
	value.reserve(4 + m_value.size());
	if(bo == BigEndian)
	{
		qToBigEndian<quint16>(m_type, (uchar *)value.data());
		qToBigEndian<quint16>(m_value.size(), (uchar *)value.data() + 2);
	}
	else
	{
		qToLittleEndian<quint16>(m_type, (uchar *)value.data());
		qToLittleEndian<quint16>(m_value.size(), (uchar *)value.data() + 2);
	}
	value += m_value;
	return value;
}

TLV TLV::fromByteArray(const char *data, int length, ByteOrder bo)
{
	TLV tlv(0xffff);
	if(length < 4)
		return tlv;
	quint16 len;
	if(bo == BigEndian)
	{
		tlv.setType(qFromBigEndian<quint16>((const uchar *)data));
		len = qFromBigEndian<quint16>((const uchar *)data + 2);
	}
	else
	{
		tlv.setType(qFromLittleEndian<quint16>((const uchar *)data));
		len = qFromLittleEndian<quint16>((const uchar *)data + 2);
	}
	if(len > length)
		tlv.setType(0xffff);
	else
		tlv.setValue(QByteArray(data + 4, len));
	return tlv;
}

TLVMap TLV::parseByteArray(const char *data, int length, ByteOrder bo)
{
	QMap<quint16, TLV> map;
	while(length > 0)
	{
		TLV tlv = TLV::fromByteArray(data, length, bo);
		if(tlv.type() == 0xffff)
			return map;
		int len = tlv.value().size() + 4;
		length -= len;
		data += len;
		map.insert(tlv.type(), tlv);
	}
	return map;
}
