/****************************************************************************
 *  xtraz.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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
#include <QXmlStreamWriter>

namespace qutim_sdk_0_3 {

namespace oscar {

class XtrazData: public Tlv2711
{
public:
	XtrazData(const QString &body, const Cookie &cookie = Cookie(true));
};

XtrazData::XtrazData(const QString &body, const Cookie &cookie) :
	Tlv2711(MsgPlugin, 0, 0, 1, cookie)
{
	appendEmptyPacket();

	// Plugin type ID
	append<quint16>(0x04f, LittleEndian); // Length
	append(Capability(0x3b60b3ef, 0xd82a6c45, 0xa4e09c5a, 0x5e67e865)); // type: xtraz script
	append<quint16>(xtrazNotify, LittleEndian); // Function ID
	append<quint32>(0x002a, LittleEndian); // Request type
	append(QString("Script Plug-in: Remote Notification Arrive"));
	// unknown
	append<quint32>(0x00000100);
	append<quint32>(0x00000000);
	append<quint32>(0x00000000);
	append<quint16>(0x0000);
	append<quint8>(0x00);

	// data
	DataUnit data;
	data.append<quint32>(body, LittleEndian);
	append<quint32>(data.data(), LittleEndian);
}

XtrazRequest::XtrazRequest(IcqContact *contact, const QString &query, const QString &notify)
{
	QString body;
	{
		QXmlStreamWriter xml(&body);
		xml.writeStartElement("N");
		xml.writeStartElement("QUERY");
		xml.writeCharacters(query);
		xml.writeEndElement();
		xml.writeStartElement("NOTIFY");
		xml.writeCharacters(notify);
		xml.writeEndElement();
		xml.writeEndElement();
	}
	XtrazData data(body);
	Cookie cookie = data.cookie();
	cookie.setContact(contact);
	cookie.lock();
	init(contact, 2, cookie);
	appendTLV(0x05, Channel2MessageData(1, data).data());
	appendTLV(0x03);
}

XtrazResponse::XtrazResponse(IcqContact *contact, const QString &response, const Cookie &cookie) :
	ServerResponseMessage(contact, 2, 3, cookie)
{
	QString body;
	{
		QXmlStreamWriter xml(&body);
		xml.writeStartElement("NR");
		xml.writeStartElement("RES");
		xml.writeCharacters(response);
		xml.writeEndElement();
		xml.writeEndElement();
	}
	XtrazData data(body, cookie);
	append(data.data());
}

} } // namespace qutim_sdk_0_3::oscar
