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
