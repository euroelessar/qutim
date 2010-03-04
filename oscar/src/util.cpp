/****************************************************************************
 *  util.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
#include <qutim/debug.h>
#include <qutim/json.h>

namespace qutim_sdk_0_3 {

namespace oscar {

namespace Util {

static QTextCodec *_asciiCodec;

class CodecWrapper : public QTextCodec
{
public:
	CodecWrapper()
	{
		utf8 = 0;
		utf8 = static_cast<CodecWrapper*>(QTextCodec::codecForName("UTF-8"));
	}
protected:
	QByteArray name() const { return utf8 ? utf8->name() + " wrapper": "wrapper"; }
	QString convertToUnicode(const char *chars, int len, ConverterState *state) const
	{
		if (Json::isValidUtf8(chars, len, true)) {
			return utf8->convertToUnicode(chars, len, state);
		} else {
			return static_cast<CodecWrapper*>(_asciiCodec)->convertToUnicode(chars, len, state);
		}
	}
	QByteArray convertFromUnicode(const QChar *input, int number, ConverterState *state) const
	{
		return utf8->convertFromUnicode(input, number, state);
	}
	int mibEnum() const { return utf8->mibEnum(); }
private:
	CodecWrapper *utf8;
};

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

extern QTextCodec *detectCodec()
{
	static CodecWrapper codec;
	return &codec;
}

} } } // namespace qutim_sdk_0_3::oscar::Util
