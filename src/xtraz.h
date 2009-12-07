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

class QXmlStreamReader;

namespace Icq {

class IcqContact;
class DataUnit;

class Xtraz
{
public:
	Xtraz();
	static void handleXtraz(IcqContact *contact, quint16 type, const DataUnit &data);
private:
	static void handleNotify(IcqContact *contact, const QString &message);
	static void parseQuery(const QString &query, QString *pluginID);
	static void parseNotify(const QString &notify, QString *reqId, QString *uin,
							QString *srvId = NULL, QString *trans = NULL);
};

} // namespace Icq

#endif // XTRAZ_H
