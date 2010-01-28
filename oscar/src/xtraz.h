/****************************************************************************
 *  xtraz.h
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef XTRAZ_H
#define XTRAZ_H

#include "icq_global.h"
#include "messages.h"

#include <QXmlStreamReader>

namespace Icq
{

class IcqContact;
class XtrazPrivate;

const Capability MSG_XSTRAZ_SCRIPT(0x3b60b3ef, 0xd82a6c45, 0xa4e09c5a, 0x5e67e865);

class XtrazRequest: public ServerMessage
{
public:
	XtrazRequest(IcqContact *contact, const QString &query, const QString &notify);
};

class XtrazResponse: public ServerResponseMessage
{
public:
	XtrazResponse(IcqContact *contact, const QString &response, const Cookie &cookie);
};

class Xtraz
{
public:
	Xtraz();
	static void handleXtraz(IcqContact *contact, quint16 type, const DataUnit &data, const Cookie &cookie);
	static bool handelXStatusCapabilities(IcqContact *contact, const Capabilities &caps, qint8 mood);
	static void removeXStatuses(Capabilities &caps);
private:
	static XtrazPrivate *data();
	static void handleNotify(IcqContact *contact, const QString &message, const Cookie &cookie);
	static void parseQuery(const QString &query, QString *pluginID);
	static void parseRes(IcqContact *contact, const QString &res);
	static void parseSrv(IcqContact *contact, QXmlStreamReader &xml, bool response, const Cookie &cookie = Cookie());
	static void parseVal(IcqContact *contact, QXmlStreamReader &xml);
	static void parseAwayMsg(IcqContact *contact, QXmlStreamReader &xml);
	static void parseRequest(IcqContact *contact, QXmlStreamReader &xml, const Cookie &cookie);
	static void sendXStatus(IcqContact *contact, const Cookie &cookie);
};

} // namespace Icq

#endif // XTRAZ_H
