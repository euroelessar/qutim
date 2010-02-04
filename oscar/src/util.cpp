/****************************************************************************
 *  util.cpp
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

#include "util.h"
#include <QCoreApplication>

namespace Icq
{

namespace Util
{
static QTextCodec *_asciiCodec;

extern QTextCodec *asciiCodec()
{
	Q_ASSERT(_asciiCodec);
	return _asciiCodec;
}

extern void setAsciiCodec(QTextCodec *codec)
{
	Q_ASSERT(codec);
	_asciiCodec = codec;
}

extern QTextCodec *utf8Codec()
{
	static QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	return codec;
}

extern QTextCodec *utf16Codec()
{
	static QTextCodec *codec = QTextCodec::codecForName("UTF-16BE");
	return codec;
}

extern QTextCodec *defaultCodec()
{
	return utf8Codec();
}

} // namespace Icq::Util

} // namespace Icq
