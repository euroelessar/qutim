/****************************************************************************
 *  util.h
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

#ifndef UTIL_H
#define UTIL_H

#include <QByteArray>
#include <QtEndian>
#include <QTextCodec>

#define IMPLEMENT_ME QString("Function %1 at line %2: implement me.").arg(Q_FUNC_INFO).arg(__LINE__)

namespace Icq
{

namespace Util
{

template<typename T>
inline QByteArray toBigEndian(T source)
{
	QByteArray data;
	data.resize(sizeof(T));
	qToBigEndian(source, (uchar *) data.data());
	return data;
}
template<typename T>
inline QByteArray toLittleEndian(T source)
{
	QByteArray data;
	data.resize(sizeof(T));
	qToLittleEndian(source, (uchar *) data.data());
	return data;
}

// SNAC(0x17,0x03), TLV(0x05)
extern QString connectionErrorText(quint16 code);

extern QTextCodec *asciiCodec();
extern void setAsciiCodec(QTextCodec *codec);
extern QTextCodec *utf8Codec();
extern QTextCodec *utf16Codec();
extern QTextCodec *defaultCodec();
extern quint64 generateCookie();

} // namespace Icq::Util

} // namespace Icq

#endif // UTIL_H
