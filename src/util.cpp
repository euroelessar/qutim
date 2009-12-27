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
#include <QDateTime>

namespace Icq
{

namespace Util
{
static QTextCodec *_asciiCodec;

QString connectionErrorText(quint16 error)
{
	switch (error) {
	case 0x01:
		return QCoreApplication::translate("ConnectionError", "Invalid nick or password");
	case 0x02:
		return QCoreApplication::translate("ConnectionError", "Service temporarily unavailable");
	case 0x04:
		return QCoreApplication::translate("ConnectionError", "Incorrect nick or password");
	case 0x05:
		return QCoreApplication::translate("ConnectionError", "Mismatch nick or password");
	case 0x06:
		return QCoreApplication::translate("ConnectionError", "Internal client error (bad input to authorizer)");
	case 0x07:
		return QCoreApplication::translate("ConnectionError", "Invalid account");
	case 0x08:
		return QCoreApplication::translate("ConnectionError", "Deleted account");
	case 0x09:
		return QCoreApplication::translate("ConnectionError", "Expired account");
	case 0x0A:
		return QCoreApplication::translate("ConnectionError", "No access to database");
	case 0x0B:
		return QCoreApplication::translate("ConnectionError", "No access to resolver");
	case 0x0C:
		return QCoreApplication::translate("ConnectionError", "Invalid database fields");
	case 0x0D:
		return QCoreApplication::translate("ConnectionError", "Bad database status");
	case 0x0E:
		return QCoreApplication::translate("ConnectionError", "Bad resolver status");
	case 0x0F:
		return QCoreApplication::translate("ConnectionError", "Internal error");
	case 0x10:
		return QCoreApplication::translate("ConnectionError", "Service temporarily offline");
	case 0x11:
		return QCoreApplication::translate("ConnectionError", " Suspended account");
	case 0x12:
		return QCoreApplication::translate("ConnectionError", "DB send error");
	case 0x13:
		return QCoreApplication::translate("ConnectionError", "DB link error");
	case 0x14:
		return QCoreApplication::translate("ConnectionError", "Reservation map error");
	case 0x15:
		return QCoreApplication::translate("ConnectionError", "Reservation link error");
	case 0x16:
		return QCoreApplication::translate("ConnectionError", "The users num connected from this IP has reached the maximum");
	case 0x17:
		return QCoreApplication::translate("ConnectionError", " The users num connected from this IP has reached the maximum (reservation)");
	case 0x18:
		return QCoreApplication::translate("ConnectionError", "Rate limit exceeded (reservation). Please try to reconnect in a few minutes");
	case 0x19:
		return QCoreApplication::translate("ConnectionError", "User too heavily warned");
	case 0x1A:
		return QCoreApplication::translate("ConnectionError", "Reservation timeout");
	case 0x1B:
		return QCoreApplication::translate("ConnectionError", "You are using an older version of ICQ. Upgrade required");
	case 0x1C:
		return QCoreApplication::translate("ConnectionError", "You are using an older version of ICQ. Upgrade recommended");
	case 0x1D:
		return QCoreApplication::translate("ConnectionError", "Rate limit exceeded. Please try to reconnect in a few minutes");
	case 0x1E:
		return QCoreApplication::translate("ConnectionError", "Can't register on the ICQ network. Reconnect in a few minutes");
	case 0x20:
		return QCoreApplication::translate("ConnectionError", "Invalid SecurID");
	case 0x22:
		return QCoreApplication::translate("ConnectionError", "Account suspended because of your age (age < 13)");
	default:
		return QCoreApplication::translate("ConnectionError", "Connection Error");
	}
}

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

extern quint64 generateCookie()
{
	return QDateTime::currentDateTime().toTime_t();
}

} // namespace Icq::Util

} // namespace Icq
