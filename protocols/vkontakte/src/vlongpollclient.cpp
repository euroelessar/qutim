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

#include "vlongpollclient.h"
#include "vconnection.h"
#include "vcontact.h"
#include "vaccount.h"
#include "vmessages.h"
#include <qutim/json.h>
#include <qutim/chatsession.h>
#include <QNetworkReply>
#include <QDateTime>
#include <QTimer>
#include <qutim/debug.h>

VLongPollClient::VLongPollClient(VConnection *connection) :
		QObject(connection), m_connection(connection)
{
	connect(m_connection, SIGNAL(connectionStateChanged(VConnectionState)),
			this, SLOT(onConnectionStateChanged(VConnectionState)));
}

VLongPollClient::~VLongPollClient()
{
}

void VLongPollClient::requestServer()
{
	QNetworkReply *reply = m_connection->get("messages.getLongPollServer");
	connect(reply, SIGNAL(finished()), this, SLOT(onServerDataReceived()));
}

void VLongPollClient::requestData(const QString &ts)
{
	QUrl url = m_url;
	url.addQueryItem("ts", ts);
	QNetworkRequest request(url);
	QNetworkReply *reply = static_cast<QNetworkAccessManager*>(m_connection)->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(onDataReceived()));
}

void VLongPollClient::onConnectionStateChanged(VConnectionState state)
{
	switch (state) {
	case Connected:
		requestServer();
		break;
	case Disconnected:
		break;
	default:
		break;
	}
}

void VLongPollClient::onServerDataReceived()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();
	debug() << Q_FUNC_INFO << rawData;
	QVariantMap data = Json::parse(rawData).toMap().value("response").toMap();
	if (data.isEmpty() || reply->error() != QNetworkReply::NoError) {
		if (m_connection->connectionState() == Connected)
			QTimer::singleShot(1000, this, SLOT(requestServer()));
		return;
	}
	QString url("http://%1?act=a_check&key=%2&wait=25");
	m_url = url.arg(data.value("server").toString(), data.value("key").toString());
	
	if (m_connection->connectionState() == Connected)
		requestData(data.value("ts").toString());
}

void VLongPollClient::onDataReceived()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	reply->deleteLater();
	QByteArray rawData = reply->readAll();
	debug() << Q_FUNC_INFO << rawData;
	QVariantMap data = Json::parse(rawData).toMap();
	if (data.contains("failed")) {
		requestServer();
		return;
	} else if (data.isEmpty() || reply->error() != QNetworkReply::NoError) {
		if (m_connection->connectionState() == Connected)
			QTimer::singleShot(1000, this, SLOT(requestServer()));
		return;
	}
	QVariantList updates = data.value("updates").toList();
	for (int i = 0; i < updates.size(); i++) {
		QVariantList update = updates.at(i).toList();
		int updateType = update.value(0, -1).toInt();
		switch (updateType) {
		case MessageAdded: {
				MessageFlags flags(update.value(2).toInt());
				if (flags & MessageOutbox)
					continue;
				QString id = update.value(3).toString();
				QString messageId = update.value(1).toString();
				QString subject = update.value(5).toString();
				QString text = update.value(6).toString();

				VContact *contact = m_connection->account()->getContact(id, true);
				qutim_sdk_0_3::Message message;
				message.setChatUnit(contact);
				message.setProperty("subject", subject);
				message.setText(unescape(text));
				message.setProperty("mid",messageId);
				//message.setProperty("html",text);
				message.setTime(QDateTime::currentDateTime());
				message.setIncoming(true);
				ChatSession *s = ChatLayer::get(contact, true);
				s->appendMessage(message);
				connect(s,SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
				m_unread_mess[s].append(message);
				contact->setChatState(ChatStateActive);
				break;
			}
		case UserOnline:
		case UserOffline: {
				// WTF? Why VKontakte sends minus as first char of id?
				QString id = update.value(1).toString().mid(1);
				VContact *contact = m_connection->account()->getContact(id, false);
				if (contact)
					contact->setOnline(updateType == UserOnline);
				break;
			}
		}
	}

	
	if (m_connection->connectionState() == Connected)
		requestData(data.value("ts").toString());
}

void VLongPollClient::onUnreadChanged(const qutim_sdk_0_3::MessageList &list)
{
	ChatSession *s = qobject_cast<ChatSession*>(sender());
	Q_ASSERT(s);
	Q_UNUSED(list);
	QStringList messageIds;
	//TODO resolve problem with containers
	MessageList unread = m_unread_mess.value(s);
	foreach (const Message &m, unread) {
		messageIds << m.property("mid").toString();
	}
	m_unread_mess[s].clear();
	m_connection->messages()->markAsRead(messageIds);
}

