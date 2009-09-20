#ifndef UTIL_H
#define UTIL_H

#include <QByteArray>
#include <QtEndian>

namespace Util
{
	template<typename T>
	inline QByteArray toBigEndian(T source)
	{
		QByteArray data;
		data.resize(sizeof(T));
		qToBigEndian(source, (uchar *)data.data());
		return data;
	}
	template<typename T>
	inline QByteArray toLittleEndian(T source)
	{
		QByteArray data;
		data.resize(sizeof(T));
		qToLittleEndian(source, (uchar *)data.data());
		return data;
	}

	// SNAC(0x17,0x03), TLV(0x05)
	extern QString connectionErrorText(quint16 code);
}

#endif // UTIL_H
