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
#include <QApplication>
#include <QDateTime>
#include <QCache>
#include <QTextCodec>

#include <qutim/debug.h>
#include <qutim/chatsession.h>
#include <qutim/notification.h>
#include <qutim/authorizationdialog.h>

#include "messages.h"

#ifndef NO_RTF_SUPPORT
#include "rtfutils.h"
#endif

#include "proto.h"
#include "mrimdefs.h"
#include "mrimcontact.h"
#include "mrimaccount.h"

struct MsgIdLink
{
	MsgIdLink(quint64 i, MrimContact *c) : msgId(i), unit(c) {}
    quint64 msgId;
	QPointer<MrimContact> unit;
};

class MessagesPrivate
{
public:
    quint32 msgSeq;
	QPointer<MrimConnection> conn;
	QCache<quint32, MsgIdLink> msgIdLink;
#ifndef NO_RTF_SUPPORT
    Rtf *rtf;
#endif
};

MrimMessages::MrimMessages(MrimConnection *conn) :
    QObject(conn), p(new MessagesPrivate)
{
    p->msgSeq = 0;
    p->conn = conn;
	p->conn.data()->registerPacketHandler(this);
	p->msgIdLink.setMaxCost(10);
#ifndef NO_RTF_SUPPORT
    p->rtf = new Rtf("cp1251");
#endif
}

MrimMessages::~MrimMessages()
{
#ifndef NO_RTF_SUPPORT
    delete p->rtf;
#endif
}

QList<quint32> MrimMessages::handledTypes()
{
    return QList<quint32>() << MRIM_CS_MESSAGE_ACK
                            << MRIM_CS_MESSAGE_STATUS
                            << MRIM_CS_OFFLINE_MESSAGE_ACK
                            << MRIM_CS_AUTHORIZE_ACK;
}

bool MrimMessages::handlePacket(class MrimPacket& packet)
{
    switch (packet.msgType()) {
    case MRIM_CS_MESSAGE_STATUS:
        handleMessageStatus(packet);
        break;
    case MRIM_CS_MESSAGE_ACK:
        handleMessageAck(packet);
        break;
	case MRIM_CS_OFFLINE_MESSAGE_ACK:
		handleOfflineMessageAck(packet);
		break;
    default:
		return false;
    }
    return true;
}

quint32 MrimMessages::sequence() const
{
    return p->msgSeq++;
}

void MrimMessages::send(MrimContact *contact, const Message &msg, Flags flags)
{
    send(contact, msg.text(), flags, msg.id());
}

void MrimMessages::sendComposingNotification(MrimContact *contact)
{
	send(contact, QLatin1String(" "), MessageFlagTypingNotify);
}

void MrimMessages::send(MrimContact *contact, const QString &text, Flags flags, qint64 id)
{
	MrimPacket msgPacket(MrimPacket::Compose);
	msgPacket.setMsgType(MRIM_CS_MESSAGE);
	msgPacket.setSequence(sequence());

	if (!(flags & (MessageFlagTypingNotify)))
		p->msgIdLink.insert(msgPacket.sequence(), new MsgIdLink(id, contact));

	msgPacket << flags;
	msgPacket.append(contact->email());
	msgPacket.append(text, true);
	msgPacket.append(" ");
	p->conn.data()->sendPacket(msgPacket);
}

void MrimMessages::handleMessageStatus(MrimPacket &packet)
{
    quint32 status = 0;
    packet.readTo(status);
    QString errString;
	MsgIdLink *msgLink = p->msgIdLink.take(packet.sequence());
	
	ChatSession *sess = msgLink ? ChatLayer::instance()->getSession(msgLink->unit.data()) : 0;
    bool delivered = false;

    switch (status) {
    case MESSAGE_DELIVERED:
        delivered = true;
        break;
    case MESSAGE_REJECTED_NOUSER:
        errString = tr("No such user");
        break;
    case MESSAGE_REJECTED_TOO_LARGE:
        errString = tr("Message too large");
        break;
    case MESSAGE_REJECTED_LIMIT_EXCEEDED:
        errString = tr("Limit exceeded");
        break;
    case MESSAGE_REJECTED_DENY_OFFMSG:
        errString = tr("User denied receiving offline messages");
        break;
    case MESSAGE_REJECTED_DENY_OFFFLSH:
        errString = tr("User denied receiving offline flash movies");
        break;
    default:
        errString = tr("Internal error");
        break;
    }

    if (sess) {
        QApplication::instance()->postEvent(sess, new MessageReceiptEvent(msgLink->msgId, delivered));
    }

    if (!errString.isEmpty()) {
        errString.prepend(tr("Message was not delivered!")+"\n");
		NotificationRequest request(Notification::System);
		request.setObject(p->conn.data()->account());
		request.setText(errString);
		request.send();
    }
}

void MrimMessages::handleMessageAck(MrimPacket &packet)
{
    quint32 msgId = 0, flags = 0;
    QString from, plainText;
    packet.readTo(msgId);
    packet.readTo(flags);
    bool isAuth = (flags & MESSAGE_FLAG_AUTHORIZE);
    bool isUnicode = !(flags & MESSAGE_FLAG_CP1251);
#ifndef NO_RTF_SUPPORT
    bool hasRtf = (flags & MESSAGE_FLAG_RTF);
#endif
    bool isTyping = (flags & MESSAGE_FLAG_NOTIFY);
    packet.readTo(&from);
    packet.readTo(&plainText, isUnicode);
	
	MrimContact *contact = p->conn.data()->account()->roster()->getContact(from, true);
	// FIXME: Add handling messages from contacts not from roster
	if (!contact)
		return;

    if (isTyping) {
		contact->updateComposingState();
        return;
    }


    if (!isAuth && !(flags & MESSAGE_FLAG_NORECV)) {
        sendDeliveryReport(from, msgId);
    }
	
	Message message;
	message.setIncoming(true);
	message.setChatUnit(contact);
	message.setTime(QDateTime::currentDateTime());
	message.setText(plainText);

#ifndef NO_RTF_SUPPORT
    if (hasRtf) {
        QString rtfMsg;
        packet.readTo(&rtfMsg);
		QString html;
        p->rtf->parse(rtfMsg, &plainText, &html);
		message.setProperty("html", html);
		if (!plainText.trimmed().isEmpty())
			message.setText(plainText);
    }
#endif
	contact->clearComposingState();
	if (isAuth) {
		QEvent *event = new Authorization::Reply(Authorization::Reply::New, contact, message.text());
		qApp->postEvent(Authorization::service(), event);
	} else {
		ChatLayer::get(contact, true)->appendMessage(message);
	}
}

QByteArray parser_read_line(char * & str, const char * & value)
{
	value = 0;
	if (!(*str))
		return QByteArray();
	char *begin = str;
	while (*str != '\r' && *str) {
		if (*str == ':' && !value) {
			*str = 0;
			value = str + 2;
		}
		str++;
	}
	char *end = str;
	if (*str)
		str += 2;
	*end = 0;
	return QByteArray::fromRawData(begin, end - begin);
}

enum ContentType
{
	ContentPlainText,
	ContentRtf,
	ContentUnknown
};

bool parser_is_boundary(const QByteArray &line, const QByteArray &boundary, bool *final)
{
	if (line == boundary) {
		*final = false;
		return true;
	} else if (line.size() == boundary.size() + 2) {
		*final = true;
		return !memcmp(line.constData() + boundary.size(), "--", 2)
				&& !memcmp(line.constData(), boundary.constData(), boundary.size());
		return true;
	}
	return false;
}

void MrimMessages::handleOfflineMessageAck(MrimPacket &packet)
{
    quint32 uidl1 = 0, uidl2 = 0;
	LPString string;
    packet.readTo(uidl1);
    packet.readTo(uidl2);
	packet.readTo(string);
	QByteArray tmpData = string.toByteArray();
	
	// Simple implementation of RFC-0822
	
	char *data = tmpData.data();
	const char *value;
	Message message;
	message.setIncoming(true);
	quint32 flags = 0;
	QByteArray boundary;
	MrimContact *contact = 0;
	// Header
	while (true) {
		QByteArray line = parser_read_line(data, value);
		if (line.isEmpty())
			break;
		if (!value)
			continue;
		if (!qstrcmp(line.constData(), "From")) {
			MrimRoster *roster = p->conn.data()->account()->roster();
			contact = roster->getContact(QLatin1String(value), true);
			message.setChatUnit(contact);
		} else if (!qstrcmp(line.constData(), "Date")) {
			QString format = QLatin1String("ddd, dd MMM yyyy hh:mm:ss");
			QString text = QLatin1String(value);
			int signPosition = text.lastIndexOf(' ');
			int delta = 0;
			if (signPosition != -1) {
				 QTime d = QTime::fromString(text.mid(signPosition + 2), QLatin1String("hhmm"));
				 if (d.isValid()) {
					 delta = (d.hour() * 60 + d.minute()) * 60;
					 if (text[signPosition + 1] != '-')
						 delta *= -1;
				 }
				 text.truncate(signPosition);
			}
			QLocale locale(QLocale::English);
			QDateTime dateTime = locale.toDateTime(text, format);
			dateTime.setTimeSpec(Qt::UTC);
			message.setTime(dateTime.addSecs(delta).toLocalTime());
		} else if (!qstrcmp(line.constData(), "X-MRIM-Flags")) {
			bool ok = true;
			flags = QByteArray::fromRawData(value, qstrlen(value)).toInt(&ok, 16);
		} else if (!qstrcmp(line.constData(), "Content-Type")) {
			boundary = QByteArray::fromRawData(value, qstrlen(value));
			boundary = boundary.mid(boundary.indexOf("boundary=") + 9);
			boundary.prepend("--", 2);
		}
	}
	if (!contact) // Smth is wrong
		return;
	// Body parts
	ContentType type;
	bool final = false;
	while (true) {
		// Try to find boundary
		QByteArray line = parser_read_line(data, value);
		if (line.isNull())
			break;
		if (!parser_is_boundary(line, boundary, &final))
			continue;
		// Part's header
		type = ContentUnknown;
		QTextCodec *codec = 0;
		while (true) {
			line = parser_read_line(data, value);
			if (line.isEmpty())
				break;
			if (!qstrcmp(line.constData(), "Content-Type")) {
				if (!qstrcmp(value, "application/x-mrim-rtf")) {
					type = ContentRtf;
				} else {
					QByteArray helper = QByteArray::fromRawData(value, qstrlen(value));
					if (helper.contains("text/plain")) {
						const char *codecName = helper.constData() + helper.indexOf("charset=") + 8;
						helper = QByteArray::fromRawData(codecName, qstrlen(codecName));
						helper.replace(';', '\0');
						debug() << "Codec" << codecName;
						codec = QTextCodec::codecForName(helper.constData());
						if (!codec)
							codec = QTextCodec::codecForName("cp1251");
						type = ContentPlainText;
					} else {
						type = ContentUnknown;
					}
				}
			} else if (!qstrcmp(line.constData(), "Content-Transfer-Encoding")) {
				// Don't know what to do
			}
		}
		// Part's body
		QByteArray helper = QByteArray::fromRawData(data, qstrlen(data));
		QByteArray body;
		int size = helper.indexOf(boundary);
		if (size == -1) {
			size = helper.size();
			body = QByteArray::fromRawData(helper.constData(), size);
		} else {
			body = QByteArray::fromRawData(helper.constData(), size - 1);
		}
		body = QByteArray::fromBase64(body);
		data += size;
		line = parser_read_line(data, value);
		if (!body.isEmpty()) {
			if (type == ContentPlainText) {
				message.setText(codec->toUnicode(body));
			}
#ifndef NO_RTF_SUPPORT
			else if (type == ContentRtf) {
				QString rtfMsg = QString::fromLatin1(body.constData(), body.size());
				QString plainText;
				QString html;
				p->rtf->parse(rtfMsg, &plainText, &html);
				message.setProperty("html", html);
			}
#endif
		}
		if (parser_is_boundary(line, boundary, &final) && final)
			break;
	}
	
	if (flags & MessageFlagAuthorize) {
		QEvent *event = new Authorization::Reply(Authorization::Reply::New, contact, message.text());
		qApp->postEvent(Authorization::service(), event);
	} else {
		ChatLayer::get(contact, true)->appendMessage(message);
	}
	
	MrimPacket deletePacket(MrimPacket::Compose);
	deletePacket.setMsgType(MRIM_CS_DELETE_OFFLINE_MESSAGE);
	deletePacket.append(uidl1);
	deletePacket.append(uidl2);
	p->conn.data()->sendPacket(deletePacket);
}

void MrimMessages::sendDeliveryReport(const QString& from, quint32 msgId)
{
    MrimPacket deliveryPacket(MrimPacket::Compose);
    deliveryPacket.setMsgType(MRIM_CS_MESSAGE_RECV);
    deliveryPacket << from;
    deliveryPacket << msgId;
    debug(DebugVerbose)<<"Sending delivery report for msg #"<<msgId<<"...";
	p->conn.data()->sendPacket(deliveryPacket);
}

