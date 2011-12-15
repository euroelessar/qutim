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

#ifndef UTIL_H
#define UTIL_H

#include "icq_global.h"
#include <QByteArray>
#include <QtEndian>
#include <QTextCodec>

#define IMPLEMENT_ME QString("Function %1 at line %2: implement me.").arg(Q_FUNC_INFO).arg(__LINE__)

namespace qutim_sdk_0_3 {

namespace oscar {

namespace Util {

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

LIBOSCAR_EXPORT QTextCodec *asciiCodec();
extern void setAsciiCodec(QTextCodec *codec);
LIBOSCAR_EXPORT QTextCodec *utf8Codec();
LIBOSCAR_EXPORT QTextCodec *utf16Codec();
LIBOSCAR_EXPORT QTextCodec *defaultCodec();
LIBOSCAR_EXPORT QTextCodec *detectCodec();

} } } // namespace qutim_sdk_0_3::oscar::Util

#endif // UTIL_H

