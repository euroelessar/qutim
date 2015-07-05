/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "xtraz.h"
#include <QXmlStreamWriter>

namespace qutim_sdk_0_3 {

namespace oscar {

class XtrazRequestPacket: public ServerMessage
{
public:
	XtrazRequestPacket(IcqContact *contact, const QString &query, const QString &notify);
};

class XtrazResponsePacket: public ServerResponseMessage
{
public:
	XtrazResponsePacket(IcqContact *contact, const QString &response, const Cookie &cookie);
};

class XtrazData: public Tlv2711
{
public:
	XtrazData(const QString &body, const Cookie &cookie = Cookie(true));
};

class XtrazDataPrivate : public QSharedData
{
public:
	QString service;
	QHash<QString, QString> data;
protected:
	void parseData(QXmlStreamReader &xml);
};

class XtrazRequestPrivate : public XtrazDataPrivate
{
public:
	QString pluginId;
	void parse(const QString &query, const QString &notify);
private:
	void parseQuery(const QString &query);
	void parseNotify(const QString &notify);
	void parseSrv(QXmlStreamReader &xml);
};

class XtrazResponsePrivate : public XtrazDataPrivate
{
public:
	QString event;
	void parse(const QString &response);
	void parseRet(QXmlStreamReader &xml);
	void parseSrv(QXmlStreamReader &xml);
	void parseVal(QXmlStreamReader &xml);
};

class XtrazPrivate : public QSharedData
{
public:
	XtrazPrivate();
	XtrazPrivate(const XtrazPrivate &data);
	XtrazPrivate &operator=(const XtrazPrivate &rhs);
	QScopedPointer<XtrazRequest> request;
	QScopedPointer<XtrazResponse> response;
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

XtrazRequestPacket::XtrazRequestPacket(IcqContact *contact, const QString &query, const QString &notify)
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
	setCookie(cookie);
	init(contact, 2, cookie);
	appendTLV(0x05, Channel2MessageData(1, data).data());
	appendTLV(0x03);
}

XtrazResponsePacket::XtrazResponsePacket(IcqContact *contact, const QString &response, const Cookie &cookie) :
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

void XtrazDataPrivate::parseData(QXmlStreamReader &xml)
{
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement())
			data.insert(xml.name().toString(), xml.readElementText());
		else if (xml.isEndElement())
			return;
	}
}

XtrazRequest::XtrazRequest(const QString &serviceId, const QString &pluginId) :
	d(new XtrazRequestPrivate)
{
	d->pluginId = pluginId;
	d->service = serviceId;
}

XtrazRequest::XtrazRequest(const XtrazRequest &data) :
	d(data.d)
{
}

XtrazRequest::~XtrazRequest()
{
}

XtrazRequest &XtrazRequest::operator=(const XtrazRequest &rhs)
{
	d = rhs.d;
	return *this;
}

QString XtrazRequest::pluginId() const
{
	return d.constData()->pluginId;
}

void XtrazRequest::setPluginId(const QString &pluginId)
{
	d.data()->pluginId = pluginId;
}

QString XtrazRequest::serviceId() const
{
	return d.constData()->service;
}

void XtrazRequest::setServiceId(const QString &serviceId)
{
	d.data()->service = serviceId;
}

QString XtrazRequest::value(const QString &name, const QString &def) const
{
	return d.constData()->data.value(name, def);
}

bool XtrazRequest::contains(const QString &name) const
{
	return d.constData()->data.contains(name);
}

void XtrazRequest::setValue(const QString &name, const QString &value)
{
	d.data()->data.insert(name, value);
}

SNAC XtrazRequest::snac(IcqContact *contact) const
{
	QString query;
	{
		QXmlStreamWriter xml(&query);
		xml.writeStartElement("Q");
		xml.writeStartElement("PluginID");
		xml.writeCharacters(d->pluginId);
		xml.writeEndElement();
		xml.writeEndElement();
		query.replace('"', '\'');
	}
	QString notify;
	{
		QXmlStreamWriter xml(&notify);
		xml.writeStartElement("srv");
		xml.writeStartElement("id");
		xml.writeCharacters(d->service);
		xml.writeEndElement();
		xml.writeStartElement("req");
		QHashIterator<QString, QString> itr(d->data);
		while (itr.hasNext()) {
			itr.next();
			xml.writeStartElement(itr.key());
			xml.writeCharacters(itr.value());
			xml.writeEndElement();
		}
		xml.writeEndElement();
		xml.writeEndElement();
		notify.replace('"', '\'');
	}
	return XtrazRequestPacket(contact, query, notify);
}

void XtrazRequestPrivate::parse(const QString &query, const QString &notify)
{
	parseQuery(query);
	if (!pluginId.isEmpty())
		parseNotify(notify);
}

void XtrazRequestPrivate::parseQuery(const QString &query)
{
	QXmlStreamReader xml(query);
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "PluginID")
				pluginId = xml.readElementText();
		}
	}
}

void XtrazRequestPrivate::parseNotify(const QString &notify)
{
	QXmlStreamReader xml(notify);
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "srv")
				parseSrv(xml);
			else
				xml.skipCurrentElement();
		} else if (xml.isEndElement()) {
			return;
		}
	}
}

void XtrazRequestPrivate::parseSrv(QXmlStreamReader &xml)
{
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "id")
				service = xml.readElementText();
			else if (xml.name() == "req")
				parseData(xml);
			else
				xml.skipCurrentElement();
		} else if (xml.isEndElement()) {
			return;
		}
	}
}

void XtrazResponsePrivate::parse(const QString &response)
{
	QXmlStreamReader xml(response);
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "ret")
				parseRet(xml);
			else
				xml.skipCurrentElement();
		} else if (xml.isEndElement()) {
			return;
		}
	}
}

void XtrazResponsePrivate::parseRet(QXmlStreamReader &xml)
{
	event = xml.attributes().value("event").toString();
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "srv")
				parseSrv(xml);
			else
				xml.skipCurrentElement();
		} else if (xml.isEndElement()) {
			return;
		}
	}
}

void XtrazResponsePrivate::parseSrv(QXmlStreamReader &xml)
{
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "id")
				service = xml.readElementText();
			else if (xml.name() == "val")
				parseVal(xml);
			else
				xml.skipCurrentElement();
		} else if (xml.isEndElement()) {
			return;
		}
	}
}

void XtrazResponsePrivate::parseVal(QXmlStreamReader &xml)
{
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "Root")
				parseData(xml);
			else
				xml.skipCurrentElement();
		} else if (xml.isEndElement()) {
			return;
		}
	}
}

XtrazResponse::XtrazResponse(const QString &serviceId, const QString &event) :
	d(new XtrazResponsePrivate)
{
	d->service = serviceId;
	d->event = event;
}

XtrazResponse::XtrazResponse(const XtrazResponse &data) :
	d(data.d)
{
}

XtrazResponse::~XtrazResponse()
{
}

XtrazResponse &XtrazResponse::operator=(const XtrazResponse &rhs)
{
	d = rhs.d;
	return *this;
}

QString XtrazResponse::event() const
{
	return d.constData()->event;
}

void XtrazResponse::setEvent(const QString &event)
{
	d.data()->event = event;
}

QString XtrazResponse::serviceId() const
{
	return d.constData()->service;
}

void XtrazResponse::setServiceId(const QString &serviceId)
{
	d.data()->service = serviceId;
}

QString XtrazResponse::value(const QString &name, const QString &def) const
{
	return d.constData()->data.value(name, def);
}

bool XtrazResponse::contains(const QString &name) const
{
	return d.constData()->data.contains(name);
}

void  XtrazResponse::setValue(const QString &name, const QString &value)
{
	d.data()->data.insert(name, value);
}

SNAC XtrazResponse::snac(IcqContact *contact, quint64 cookie) const
{
	QString response;
	{
		QXmlStreamWriter xml(&response);
		xml.writeStartElement("ret");
		xml.writeAttribute("event", d->event);
		xml.writeStartElement("srv");
		xml.writeStartElement("id");
		xml.writeCharacters(d->service);
		xml.writeEndElement();
		xml.writeStartElement("val");
		xml.writeAttribute("srv_id", d->service);
		xml.writeStartElement("Root");
		QHashIterator<QString, QString> itr(d->data);
		while (itr.hasNext()) {
			itr.next();
			xml.writeStartElement(itr.key());
			xml.writeCharacters(itr.value());
			xml.writeEndElement();
		}
		xml.writeEndElement();
		xml.writeEndElement();
		xml.writeEndElement();
		xml.writeEndElement();
		response.replace('"', '\'');
	}
	return XtrazResponsePacket(contact, response, cookie);
}

XtrazPrivate::XtrazPrivate()
{
}

XtrazPrivate::XtrazPrivate(const XtrazPrivate &data)
	: QSharedData(data)
{
	if (data.request) {
		request.reset(new XtrazRequest);
		*request = *data.request;
	}
	if (data.response) {
		response.reset(new XtrazResponse);
		*response = *data.response;
	}
}

XtrazPrivate &XtrazPrivate::operator=(const XtrazPrivate &rhs)
{
	if (rhs.request) {
		request.reset(new XtrazRequest);
		*request = *rhs.request;
	}
	if (rhs.response) {
		response.reset(new XtrazResponse);
		*response = *rhs.response;
	}
	return *this;
}

Xtraz::Xtraz(const QString &message) :
	d(new XtrazPrivate)
{
	qDebug() << Q_FUNC_INFO << message;
	QXmlStreamReader xml(message);
	QString query;
	QString notify;
	QString response;
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "QUERY")
				query = xml.readElementText();
			else if (xml.name() == "NOTIFY")
				notify = xml.readElementText();
			else if (xml.name() == "RES")
				response = xml.readElementText();
		}
	}
	if (!query.isEmpty() && !notify.isEmpty()) {
		d->request.reset(new XtrazRequest());
		d->request->d->parse(query, notify);
	} else if (!response.isEmpty()) {
		d->response.reset(new XtrazResponse());
		d->response->d->parse(response);
	}
}

Xtraz::~Xtraz()
{
}

Xtraz &Xtraz::operator=(const Xtraz &rhs)
{
	d = rhs.d;
	return *this;
}

Xtraz::Type Xtraz::type()
{
	if (d->request)
		return Request;
	else if (d->response)
		return Response;
	else
		return Invalid;
}

XtrazRequest Xtraz::request()
{
	return *d->request;
}

XtrazResponse Xtraz::response()
{
	return *d->response;
}

Xtraz::Xtraz()
{
}

Xtraz::Xtraz(const Xtraz &xtraz)
{
	d = xtraz.d;
}

} } // namespace qutim_sdk_0_3::oscar

