/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "vmessages.h"
#include "vmessages_p.h"
#include "vrequest.h"
#include "vconnection.h"
#include "vaccount.h"
#include <QNetworkReply>
#include <qutim/debug.h>
#include <QtScript/QScriptEngine>
#include <qutim/message.h>
#include <QDateTime>
#include "vcontact.h"
#include <qutim/chatsession.h>
#include <qutim/json.h>
#include <QTextDocument>
#include <QApplication>

void VMessagesPrivate::onHistoryRecieved()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	QByteArray rawData = reply->readAll();
	qDebug() << rawData;
	QVariantList data = Json::parse(rawData).toMap().value("response").toList();
	for (int i = 1; i < data.size(); i++) {
		QVariantMap map = data.at(i).toMap();
		QString html = map.value("body").toString();
		Message mess;
		mess.setText(unescape(html));
		mess.setProperty("html", html);
		mess.setProperty("subject", map.value("topic"));
		mess.setIncoming(true);
		QString id = map.value("uid").toString();
		mess.setProperty("mid",map.value("mid"));
		QDateTime time = QDateTime::fromTime_t(map.value("date").toInt());
		mess.setTime(time);
		VContact *contact = connection->account()->getContact(id, false);
		if (!contact) {
			QString name = map.value("user_name").toString();
			contact = connection->account()->getContact(id, true);
			contact->setContactName(name);
		}
		mess.setChatUnit(contact);
		ChatSession *s = ChatLayer::get(contact, true);
		s->appendMessage(mess);
		unreadMessages[s].append(mess);
		connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
	}
}

void VMessagesPrivate::onMessageSended()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	QVariantMap data = Json::parse(reply->readAll()).toMap();
	
	Message message = reply->property("message").value<Message>();
	if (message.chatUnit()) {
		ChatSession *s = ChatLayer::get(const_cast<ChatUnit *>(message.chatUnit()), false);
		if (s) {
			bool success = (reply->error() == QNetworkReply::NoError)
						   && data.contains("response");
			QApplication::instance()->postEvent(s, new MessageReceiptEvent(message.id(), success));
		}
	}	
}

void VMessages::markAsRead(const QStringList &messages)
{
	QVariantMap data;
	data.insert("mids", messages);
	d_func()->connection->get("messages.markAsRead", data);
}

void VMessagesPrivate::onConnectStateChanged(VConnectionState state)
{
	Q_Q(VMessages);
	switch (state) {
		case Connected: {
			q->getHistory();
			break;
		}
		case Disconnected: {
			}
			break;
		default:
			break;
	}
}

VMessages::VMessages(VConnection* connection): QObject(connection),
	d_ptr(new VMessagesPrivate)
{
	Q_D(VMessages);
	d->q_ptr = this;
	d->connection = connection;
	connect(connection, SIGNAL(connectionStateChanged(VConnectionState)), d, SLOT(onConnectStateChanged(VConnectionState)));
}

VMessages::~VMessages()
{
}

void VMessages::getLastMessages(int count)
{
	if (count < 1)
		return;
}

void VMessages::getHistory()
{
	Q_D(VMessages);
	QVariantMap data;
	data.insert("filters", "1");
	data.insert("preview_length", "0");
	QNetworkReply *reply = d->connection->get("messages.get", data);
	connect(reply, SIGNAL(finished()), d, SLOT(onHistoryRecieved()));
}

void VMessages::sendMessage(const Message& message)
{
	Q_D(VMessages);
	QVariantMap data;
	data.insert("uid", message.chatUnit()->id());
	QString html = message.text();
	data.insert("message", html);
	QNetworkReply *reply = d->connection->get("messages.send", data);
	reply->setProperty("message",qVariantFromValue<Message>(message));
	connect(reply, SIGNAL(finished()), d, SLOT(onMessageSended()));
}

ConfigGroup VMessages::config()
{
	return d_func()->connection->config("messages");
}

void VMessagesPrivate::onUnreadChanged(const qutim_sdk_0_3::MessageList &list)
{
	ChatSession *s = qobject_cast<ChatSession*>(sender());
	Q_ASSERT(s);
	Q_UNUSED(list);
	QStringList messageIds;
	//TODO resolve problem with containers
	MessageList unread = unreadMessages.value(s);
	foreach (const Message &m, unread) {
		messageIds << m.property("mid").toString();
	}
	unreadMessages[s].clear();
	connection->messages()->markAsRead(messageIds);
}

