/****************************************************************************
 *  xtraz.cpp
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

#include "xtraz.h"
#include "dataunit.h"
#include "icqcontact.h"
#include <QXmlStreamReader>
#include <QDebug>

namespace Icq {

enum XtrazType
{
	xtrazInvitation = 0x0001,
	xtrazData       = 0x0002,
	xtrazUserRemove = 0x0004,
	xtrazNotify     = 0x0008
};

Xtraz::Xtraz()
{
}

void Xtraz::handleXtraz(IcqContact *contact, quint16 type, const DataUnit &data)
{
	QString message = data.readData<quint32>(DataUnit::LittleEndian);
	if(type == xtrazNotify)
		handleNotify(contact, message);
	else
		qDebug() << "Unhandled xtraz message" << type << message;
}

void Xtraz::handleNotify(IcqContact *contact, const QString &message)
{
	QString pluginId;
	QString request;
	QString uin;
	QXmlStreamReader xml(message);
	while (!xml.atEnd()) {
		xml.readNext();
		if(xml.isStartElement())
		{
			if(xml.name() == "QUERY")
				parseQuery(xml.readElementText(), &pluginId);
			else if(xml.name() == "NOTIFY")
				parseNotify(xml.readElementText(), &request, &uin);
		}
	}
	if (xml.hasError())
		qDebug() << "Xtraz parsing error" << xml.errorString();

	if(pluginId == "srvMng" && request == "AwayStat")
		qDebug() << uin << "requested my away message";
}

void Xtraz::parseQuery(const QString &query, QString *pluginId)
{
	QXmlStreamReader xml(query);
	while (!xml.atEnd())
	{
		xml.readNext();
		if(xml.isStartElement())
		{
			if(xml.name() == "PluginID")
				*pluginId = xml.readElementText();
		}
	}

	if (xml.hasError())
		qDebug() << "Parsing error of the xtraz query" << xml.errorString();
}

void Xtraz::parseNotify(const QString &notify, QString *reqId, QString *uin,
						QString *srvId, QString *trans)
{
	enum State
	{
		root,
		srv,
		req
	} state = root;
	int depth = 0;
	QString *data = NULL;
	QXmlStreamReader xml(notify);

	while (!xml.atEnd())
	{
		xml.readNext();
		if(xml.isStartElement())
		{
			++depth;
			if(state == root && depth == 1 && xml.name() == "srv")
			{
				state = srv;
			}
			else if(state == srv && depth == 2)
			{
				if(xml.name() == "id")
					data = srvId;
				else if(xml.name() == "req")
					state = req;
			}
			else if(state == req && depth == 3)
			{
				if(xml.name() == "id")
					data = reqId;
				else if(xml.name() == "trans")
					data = trans;
				else if(xml.name() == "senderId")
					data = uin;
			}
		}
		else if(xml.isEndElement())
		{
			if(state == srv && depth == 1 && xml.name() == "srv")
				state = root;
			else if(state == req && depth == 2 && xml.name() == "req")
				state = srv;
			--depth;
			data = NULL;
		}
		else if(xml.isCharacters() && data)
		{
			*data = xml.text().toString();
		}
	}
	if (xml.hasError())
		qDebug() << "Parsing error of the xtraz notify" << xml.errorString();
}

} // namespace Icq
