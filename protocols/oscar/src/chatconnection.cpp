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

#include "chatconnection.h"
#include "icqcontact.h"
#include "oscarchat.h"
#include "icqaccount.h"
#include "messages.h"
#include "buddycaps.h"
#include <QTextDocument>
#include <qutim/chatsession.h>
#include <QCoreApplication>

namespace qutim_sdk_0_3 {
namespace oscar {

ChatConnection::ChatConnection(const TLVMap &data, IcqAccount *account)
    : AbstractConnection(account, account)
{
	m_infos << SNACInfo(ServiceFamily, ServerRedirectService)
	        << SNACInfo(ChatFamily, RoomInfoUpdate)
	        << SNACInfo(ChatFamily, UsersJoined)
	        << SNACInfo(ChatFamily, UsersLeft)
	        << SNACInfo(ChatFamily, ChannelMsgToClient)
	        << SNACInfo(ChatFamily, UpdatePreferences)
	        << SNACInfo(ChatFamily, SetRoomOwner);
	registerHandler(this);
	
	QList<QByteArray> list = data.value(0x05).data().split(':');
	m_cookie = data.value(0x06).data();
	socket()->connectToHost(list.at(0), list.size() > 1 ? list.at(1).toInt() : 5190);
	connect(this, SIGNAL(disconnected()), SLOT(deleteLater()));
}

void ChatConnection::processNewConnection()
{
	AbstractConnection::processNewConnection();
	FLAP flap(0x01);
	flap.append<quint32>(0x01);
	flap.appendTLV<QByteArray>(0x0006, m_cookie);
	m_cookie.clear();
	send(flap);
}

void ChatConnection::processCloseConnection()
{
	AbstractConnection::processCloseConnection();
}

void ChatConnection::handleSNAC(AbstractConnection *conn, const SNAC &snac)
{
	Q_ASSERT(this == conn);
	AbstractConnection::handleSNAC(this, snac);
	snac.resetState();
	switch ((snac.family() << 16) | snac.subtype()) {
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		SNAC snac(ServiceFamily, ServiceClientReady);
		snac.append(QByteArray::fromHex(
				"0001 0004 0110 164f" // ServiceFamily
				"000e 0001 0110 164f")); // ChatFamily
		send(snac);
		setState(Connected);
		break;
	}
	case ChatFamily << 16 | UsersJoined:
	case ChatFamily << 16 | UsersLeft: {
		debug() << Q_FUNC_INFO << snac.data().toHex();
		ChatSession *session = 0;
		if (!m_roomId.cookie.isEmpty()) {
			OscarChat *chat = account()->findChat(m_roomId.cookie);
			session = ChatLayer::get(chat, true);
		}
		QList<IcqContact*> contacts;
		while (snac.dataSize() > 0) {
			QString username = snac.read<QString, quint8>();
			quint16 warningLevel = snac.read<quint16>();
			Q_UNUSED(warningLevel);
			TLVMap tlvs = snac.read<TLVMap, quint16>();
			debug() << snac.subtype() << username << tlvs;
			IcqContact *contact = account()->getContact(username, true);
			if (snac.subtype() == UsersJoined) {
				m_participants << contact;
				if (session)
					session->addContact(contact);
			} else {
				m_participants.removeOne(contact);
				if (session)
					session->removeContact(contact);
			}
			contacts << contact;
		}
		if (snac.subtype() == UsersJoined)
			emit usersJoined(contacts);
		else
			emit usersLeaved(contacts);
		break;
	}
	case ChatFamily << 16 | ChannelMsgToClient: {
		Cookie cookie = snac.read<Cookie>();
		quint16 channel = snac.read<quint16>();
		TLVMap tlvs = snac.read<TLVMap>();
		QString uin = tlvs.value(0x0003).read<QString, quint8>();
		OscarChat *chat = account()->findChat(m_roomId.cookie);
		if (uin == account()->id()) {
			quint64 id = m_delivery.take(cookie.id());
			qApp->postEvent(ChatLayer::get(chat, true), new MessageReceiptEvent(id, true));
			return;
		}
		
		TLVMap data = tlvs.value<TLVMap>(0x0005);
		debug() << Q_FUNC_INFO << cookie.id() << channel << tlvs << data;
		
		IcqContact *contact = account()->getContact(uin, true);
		QByteArray text = data.value<QByteArray>(0x0001);
		QByteArray codecName = data.value<QByteArray>(0x0002);
		QTextCodec *codec = QTextCodec::codecForName(codecName);
		QString html = (codec ? codec : Util::defaultCodec())->toUnicode(text);
		Message msg(unescape(html));
		msg.setProperty("html", html);
		msg.setChatUnit(chat);
		msg.setProperty("senderName", contact->title());
//		if (user)
		msg.setProperty("senderId", uin);
//		if (!msg.text().contains(d->room->nick()))
//			msg.setProperty("silent", true);
		msg.setIncoming(true);
		msg.setTime(QDateTime::currentDateTime());
		ChatLayer::get(chat, true)->append(msg);
		break;
	}
	case ChatFamily << 16 | RoomInfoUpdate: {
		m_roomId = snac.read<RoomId>();
		quint8 detailLevel = snac.read<quint8>();
		Q_UNUSED(detailLevel);
		TLVMap roomTLVs = snac.read<TLVMap, quint16>();
		debug() << "Join to room" << roomTLVs;
		QString title = roomTLVs.value<QString>(0x006a);
		OscarChat *chat = account()->findChat(m_roomId.cookie);
		chat->attach(this);
		debug() << m_title << title;
		if (m_title != title) {
			qSwap(m_title, title);
			emit titleChanged(m_title, title);
		}
		break;
	}
	}
}

void ChatConnection::sendMessage(const Message &msg)
{
	Cookie cookie(true);
	m_delivery.insert(cookie.id(), msg.id());
	SNAC snac(ChatFamily, ChannelMsgToHost);
	snac.append(cookie);
	snac.append<quint16>(0x0003);
	snac.appendTLV(ChatToRoom);
	snac.appendTLV(RequestReflection);
	TLVMap data;
	TLV text(0x0001);
	text.append(Qt::escape(msg.text()).replace('\n', QLatin1String("<br/>")), Util::utf8Codec());
	data.insert(text);
	data.insert(0x0002, Util::utf8Codec()->name());
	snac.appendTLV(0x0005, data);
	send(snac);
	debug() << Q_FUNC_INFO << cookie.id();
}

void ChatConnection::inviteUser(IcqContact *contact, const QString &reason)
{
	Cookie cookie(true);
	Channel2BasicMessageData data(MsgRequest, ICQ_CAPABILITY_AIMCHAT, cookie);
	data.appendTLV<quint16>(0x000a, 0x0001);
	data.appendTLV(0x000f);
	data.appendTLV(0x000c, reason);
	data.appendTLV(0x2711, m_roomId);
	ServerMessage message(contact, data);
	account()->connection()->send(message);
}
}
}
