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

#include "miranda.h"
#include <QFileInfo>
#include <QTextDocument>
#include <QTextCodec>
#include <qutim/icon.h>
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

namespace Miranda {

const char *DBHEADER_SIGNATURE = "Miranda ICQ DB";

miranda::miranda()
{
}

miranda::~miranda()
{
}

inline DWORD ReadDWord(const BYTE * &data)
{
	DWORD a = *(data++);
	a += (*(data++) << 8);
	a += (*(data++) << 16);
	a += (*(data++) << 24);
	return a;
}

inline WORD ReadWord(const BYTE * &data)
{
	WORD a = *(data++);
	a += (*(data++) << 8);
	return a;
}

inline BYTE ReadByte(const BYTE * &data)
{
	return *(data++);
}

inline QByteArray ReadByteArray(const BYTE * &data)
{
	WORD lenth = ReadWord(data);
	QByteArray result((const char *)data, lenth);
	data += lenth;
	return result;
}

static void ReadDBHeader(DBHeader *header, const BYTE *data)
{
	for(int i = 0; i < 16; i++)
		header->signature[i] = ReadByte(data);
	header->version = ReadDWord(data);
	header->ofsFileEnd = ReadDWord(data);
	header->slackSpace = ReadDWord(data);
	header->contactCount = ReadDWord(data);
	header->ofsFirstContact = ReadDWord(data);
	header->ofsUser = ReadDWord(data);
	header->ofsFirstModuleName = ReadDWord(data);
}

static void ReadDBContact(DBContact *contact, const BYTE *data)
{
	contact->signature = ReadDWord(data);
	contact->ofsNext = ReadDWord(data);
	contact->ofsFirstSettings = ReadDWord(data);
	contact->eventCount = ReadDWord(data);
	contact->ofsFirstEvent = ReadDWord(data);
	contact->ofsLastEvent = ReadDWord(data);
	contact->ofsFirstUnreadEvent = ReadDWord(data);
	contact->timestampFirstUnread = ReadDWord(data);
}

static void ReadDBEvent(DBEvent *event, const BYTE *data)
{
	event->signature = ReadDWord(data);
	event->ofsPrev = ReadDWord(data);
	event->ofsNext = ReadDWord(data);
	event->ofsModuleName = ReadDWord(data);
	event->timestamp = ReadDWord(data);
	event->flags = ReadDWord(data);
	event->eventType = ReadWord(data);
	event->cbBlob = ReadDWord(data);
	event->blob = QByteArray((const char *)data, event->cbBlob);
}

static void ReadDBModuleName(DBModuleName *module_name, const BYTE *data)
{
	module_name->signature = ReadDWord(data);
	module_name->ofsNext = ReadDWord(data);
	module_name->cbName = ReadByte(data);
	module_name->name = QByteArray((const char *)data, module_name->cbName);
	module_name->name.append((char)0);
}

static void ReadDBContactSettings(DBContactSettings *contact_settings, const BYTE *data)
{
	contact_settings->signature = ReadDWord(data);
	contact_settings->ofsNext = ReadDWord(data);
	contact_settings->ofsModuleName = ReadDWord(data);
	contact_settings->cbBlob = ReadDWord(data);
	contact_settings->blob = QByteArray((const char *)data, contact_settings->cbBlob);
}

static QVariant GetVariant(const BYTE * &data, QTextDecoder *decoder)
{
	BYTE type = ReadByte(data);
	switch(type)
	{
	case DBVT_DELETED:
		return QVariant();
	case DBVT_BYTE:
		return ReadByte(data);
	case DBVT_WORD:
		return ReadWord(data);
	case DBVT_DWORD:
		return ReadDWord(data);
	case DBVT_ASCIIZ:
		return decoder->toUnicode(ReadByteArray(data));
	case DBVT_UTF8:
		return QString::fromUtf8(ReadByteArray(data));
	case DBVT_WCHAR: {
		WORD length = ReadWord(data);
        WCHAR *array = (WCHAR *)malloc(length * sizeof(WORD));
		for(int i = 0; i < length; i++)
			array[i] = ReadWord(data);
		QString result = QString::fromUtf16(array, length);
        free(array);
		return result;
	}
	case DBVT_BLOB:
		return ReadByteArray(data);
	default:
		return QVariant();
	}
}

static QHash<QString, QVariant> GetSettings(const DBContact &contact, const QByteArray &module, const BYTE *data, QTextDecoder *decoder)
{
	DBContactSettings contact_settings;
	DWORD offset = contact.ofsFirstSettings;
	QHash<QString, QVariant> result;
	while(offset)
	{
		ReadDBContactSettings(&contact_settings, data + offset);
		DBModuleName module_name;
		ReadDBModuleName(&module_name, data + contact_settings.ofsModuleName);
		if(QLatin1String(module_name.name) == QLatin1String(module))
		{
			const BYTE *data = (const BYTE *)contact_settings.blob.constData();
			while(true)
			{
				BYTE length = ReadByte(data);
				QByteArray key = QByteArray((const char *)data, length);
				data += length;
				if(key.isEmpty())
					break;
				QVariant value = GetVariant(data, decoder);
				if(!value.isNull())
					result.insert(QString::fromLatin1(key, key.size()).toLower(), value);
			}
		}
		offset = contact_settings.ofsNext;
	}
	return result;
}

static bool IsSupported(const QByteArray &protocol)
{
	if(protocol.startsWith("JABBER")
		|| protocol.startsWith("ICQ")
		|| protocol.startsWith("MSN")
		|| protocol.startsWith("AIM")
		|| protocol.startsWith("GG")
		|| protocol.startsWith("IRC")
		|| protocol.startsWith("YAHOO"))
		return true;
	return false;
}

static QString Miranda2qutIM(const QByteArray &protocol)
{
	if(protocol.startsWith("JABBER"))
		return "jabber";
	if(protocol.startsWith("ICQ"))
		return "icq";
	if(protocol.startsWith("MSN"))
		return "msn";
	if(protocol.startsWith("AIM"))
		return "aim";
	if(protocol.startsWith("GG"))
		return "gadu-gadu";
	if(protocol.startsWith("IRC"))
		return "irc";
	if(protocol.startsWith("YAHOO"))
		return "yahoo";
	return QString();
}

static QString GetID(const QHash<QString, QVariant> &settings, const QByteArray &protocol)
{
	if(protocol.startsWith("JABBER"))
		return settings.value("jid").toString();
	if(protocol.startsWith("ICQ"))
		return settings.value("uin").toString();
	if(protocol.startsWith("MSN"))
		return settings.value("e-mail").toString();
	if(protocol.startsWith("AIM"))
		return settings.value("sn").toString();
	if(protocol.startsWith("GG"))
		return settings.value("uin").toString();
	if(protocol.startsWith("IRC"))
		return settings.value("nick").toString();
	if(protocol.startsWith("YAHOO"))
		return settings.value("yahoo_id").toString();
	return QString();
}

void miranda::loadMessages(const QString &path)
{
	QFileInfo info(path);
	if(!info.exists() || !info.isFile())
		return;
	QFile file(path);
	if(!file.open(QIODevice::ReadOnly))
		return;
	QTextDecoder *decoder = QTextCodec::codecForName(charset())->makeDecoder();
	QByteArray bytes;
	const BYTE *data_begin = (BYTE *)file.map(0, file.size());
	if(!data_begin)
	{
		bytes = file.readAll();
		data_begin = (BYTE *)bytes.constData();
	}
	DBHeader header;
	const BYTE *data = data_begin;
	ReadDBHeader(&header, data);
	if(QLatin1String((const char *)header.signature) != QLatin1String(DBHEADER_SIGNATURE))
		return;
	setMaxValue(header.contactCount);
	int offset = header.ofsFirstContact;
	DBContact contact;
	DBContact user;
	ReadDBContact(&user, data + header.ofsUser);
	int num = 0;
	while(offset)
	{
		ReadDBContact(&contact, data + offset);
		setValue(++num);
		if (contact.ofsFirstEvent) {
			QHash<QString, QVariant> protocol_settings = GetSettings(contact, "Protocol", data, decoder);
			QByteArray protocol = protocol_settings.value("p").toByteArray();
			if (IsSupported(protocol)) {
				setProtocol(Miranda2qutIM(protocol));
				QHash<QString, QVariant> contact_settings = GetSettings(contact, protocol, data, decoder);
				QHash<QString, QVariant> account_settings = GetSettings(user, protocol, data, decoder);
				setAccount(GetID(account_settings, protocol));
				setContact(GetID(contact_settings, protocol));
				DBEvent event;
				offset = contact.ofsFirstEvent;
				while (offset) {
					ReadDBEvent(&event, data + offset);
					offset = event.ofsNext;
					if(event.eventType != EVENTTYPE_MESSAGE)
						continue;
					Message message;
					message.setIncoming(!(event.flags & DBEF_SENT));
					message.setTime(QDateTime::fromTime_t(event.timestamp));
					int end = event.blob.indexOf('\0');
					QByteArray blob = end == -1 ? event.blob : event.blob.left(end);
					message.setText((event.flags & DBEF_UTF) ? QString::fromUtf8(blob) : decoder->toUnicode(blob));
					appendMessage(message);
				}
			} else {
				QHash<QString, QVariant> contact_settings = GetSettings(contact, protocol, data, decoder);
				warning() << "Unknown protocol:" << protocol << contact_settings.keys();
			}
		}
		offset = contact.ofsNext;
	}
}

bool miranda::validate(const QString &path)
{
	QFileInfo info(path);
	if(info.exists() && info.isFile())
	{
		QFile file(path);
		if(!file.open(QIODevice::ReadOnly))
			return false;
		QByteArray bytes;
		const BYTE *data_begin = (BYTE *)file.map(0, file.size());
		if(!data_begin)
		{
			bytes = file.read(sizeof(DBHeader)*2);
			data_begin = (BYTE *)bytes.constData();
		}
		DBHeader header;
		const BYTE *data = data_begin;
		ReadDBHeader(&header, data);
		if(QLatin1String((const char *)header.signature) != QLatin1String(DBHEADER_SIGNATURE))
			return false;
		return true;
	}
	return false;
}

QString miranda::name()
{
	return "Miranda IM";
}

QIcon miranda::icon()
{
	return Icon("miranda");
}

}

}

