/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#ifndef CHATCONNECTION_H
#define CHATCONNECTION_H

#include "icq_global.h"
#include "connection.h"
#include "chatnavigation.h"
#include <qutim/message.h>

namespace qutim_sdk_0_3 {
namespace oscar {

class OscarChat;

class ChatMessage
{
public:
	inline ChatMessage(const QString &text = QString()) : m_text(text) {}
	inline ChatMessage &operator =(const QString &text) { m_text = text; return *this; }
	
	inline operator QString() const { return m_text; }
	
private:
	QString m_text;
};

template<>
struct toDataUnitHelper<ChatMessage>
{
	static inline QByteArray toByteArray(const ChatMessage &msg)
	{
		DataUnit data;
		data.appendTLV<QByteArray>(0x0001, Util::utf16Codec()->fromUnicode(msg));
		data.appendTLV<QByteArray>(0x0002, QByteArray("unicode-2-0"));
		return QByteArray(data);
	}
};

template<>
struct fromDataUnitHelper<ChatMessage>
{
	static inline ChatMessage fromByteArray(const DataUnit &d)
	{
		TLVMap data = d.read<TLVMap>();
		QByteArray text = data.value<QByteArray>(0x0001);
		QByteArray codecName = data.value<QByteArray>(0x0002);
		QTextCodec *codec = 0;
		if (codecName == "unicode-2-0")
			codec = Util::utf16Codec();
		else if (codecName == "us-ascii")
			codec = QTextCodec::codecForName("latin-1");
		else
			codec = QTextCodec::codecForName(codecName);
		if (!codec)
			codec = Util::defaultCodec();
		return codec->toUnicode(text);
	}
};

class ChatConnection : public AbstractConnection
{
	Q_OBJECT
public:
	enum ChatMessageType
	{
		ChatToRoom          = 0x0001,
		WhisperToUser       = 0x0002,
		Sender              = 0x0003,
		Data                = 0x0005,
		RequestReflection   = 0x0006,
		StageData           = 0x0008,
		SenderName          = 0x0009
	};
	
    ChatConnection(const TLVMap &data, IcqAccount *account);
	
	void sendMessage(const Message &msg);
	void inviteUser(IcqContact *contact, const QString &reason = QString());
	QList<IcqContact*> participants() const { return m_participants; }
	QString title() const { return m_title; }

protected:
	virtual void processNewConnection();
	virtual void processCloseConnection();
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);

signals:
	void usersJoined(const QList<qutim_sdk_0_3::oscar::IcqContact*> &contacts);
	void usersLeaved(const QList<qutim_sdk_0_3::oscar::IcqContact*> &contacts);
	void titleChanged(const QString &current, const QString &previous);

private:
	RoomId m_roomId;
	QString m_title;
	QByteArray m_cookie;
	QHash<quint64, quint64> m_delivery;
	QList<IcqContact*> m_participants;
};

}
}

#endif // CHATCONNECTION_H
