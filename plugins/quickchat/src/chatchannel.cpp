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

#include "chatchannel.h"
#include "chat.h"
#include "chatchannelusersmodel.h"
#include <QTextDocument>
#include <qutim/thememanager.h>
#include <qutim/conference.h>
#include <qutim/notification.h>
#include <qutim/config.h>
#include <qutim/history.h>
#include <QDateTime>
#include <QMetaMethod>
#include <QUuid>
#include <QUrlQuery>

namespace QuickChat
{
using namespace qutim_sdk_0_3;

ChatChannel::ChatChannel(qutim_sdk_0_3::ChatUnit *unit)
	: ChatSession(Chat::instance()), m_unit(unit), m_page(0)
{
    m_id = QUuid::createUuid().toString();
	m_model = new ChatMessageModel(this);
	m_units = new ChatChannelUsersModel(this);
	if (Conference *conf = qobject_cast<Conference *>(unit)) {
		foreach (ChatUnit *u, conf->lowerUnits()) {
			if (Buddy *buddy = qobject_cast<Buddy*>(u))
				addContact(buddy);
		}
	}
}

ChatChannel::~ChatChannel()
{
	setActive(false);
	Chat *chat = static_cast<Chat*>(Chat::instance());
	if (chat && chat->activeSession() == this)
        chat->setActiveSession(0);
}

qutim_sdk_0_3::ChatUnit *ChatChannel::getUnit() const
{
    return m_unit;
}

void ChatChannel::setChatUnit(qutim_sdk_0_3::ChatUnit* unit)
{
	m_unit = unit;
}

QTextDocument *ChatChannel::getInputField()
{
	return 0;
}

void ChatChannel::markRead(quint64 id)
{
	if (id == Q_UINT64_C(0xffffffffffffffff)) {
		m_unread.clear();
		emit unreadChanged(m_unread);
		emit unreadCountChanged(m_unread.count());
		return;
	}
	for (int i = 0; i < m_unread.size(); ++i) {
		if (m_unread.at(i).id() == id) {
			m_unread.removeAt(i);
			emit unreadChanged(m_unread);
			emit unreadCountChanged(m_unread.count());
			return;
		}
	}
}

qutim_sdk_0_3::MessageList ChatChannel::unread() const
{
	return m_unread;
}

int ChatChannel::unreadCount() const
{
	return m_unread.count();
}

void ChatChannel::addContact(qutim_sdk_0_3::Buddy *c)
{
	m_units->addUnit(c);
}

void ChatChannel::removeContact(qutim_sdk_0_3::Buddy *c)
{
	m_units->removeUnit(c);
}

QObject *ChatChannel::model() const
{
	return m_model;
}

void ChatChannel::send(const QString &text)
{    
    if (text.trimmed().isEmpty())
        return;

	Message message(text);
	message.setIncoming(false);
	message.setChatUnit(m_unit);
	message.setTime(QDateTime::currentDateTime());
    appendMessage(message);
}

void ChatChannel::showChat()
{
	static_cast<Chat*>(Chat::instance())->show();
}

void ChatChannel::close()
{
	static_cast<Chat*>(Chat::instance())->handleSessionDeath(this);
    deleteLater();
}

void ChatChannel::clear()
{
    emit clearRequested();
}

QObject *ChatChannel::units() const
{
	return m_units;
}

QObject *ChatChannel::page() const
{
	return m_page;
}

void ChatChannel::setPage(QObject *page)
{
	if (m_page != page) {
		m_page = page;
		emit pageChanged(page);
	}
}

bool ChatChannel::supportJavaScript() const
{
    return m_javaScriptListeners > 0;
}

QString ChatChannel::id() const
{
    return m_id;
}

bool ChatChannel::event(QEvent *ev)
{
    if (ev->type() == MessageReceiptEvent::eventType()) {
        MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
        emit receivedMessageReceipt(msgEvent->id(), msgEvent->success());
    }
    return ChatSession::event(ev);
}

QVariant ChatChannel::evaluateJavaScript(const QString &script)
{
    emit javaScriptRequest(script);
    return QVariant();
}

QString ChatChannel::htmlEscape(const QString &text)
{
    return text.toHtmlEscaped();
}

void ChatChannel::appendText(const QString &text)
{
    emit appendTextRequested(text);
}

void ChatChannel::appendNick(const QString &nick)
{
    emit appendNickRequested(nick);
}

void ChatChannel::loadHistory()
{
    Config config(QStringLiteral("appearance"));
    config.beginGroup(QStringLiteral("chat/history"));
    int maxDisplayCount = config.value(QStringLiteral("maxDisplayMessages"), 5);

    auto result = History::instance()->read(unit(), maxDisplayCount);
    result.connect(this, [this] (MessageList messages) {
        for (Message &message : messages) {
            message.setProperty("silent", true);
            message.setProperty("store", false);
            message.setProperty("history", true);
            if (!message.chatUnit()) //TODO FIXME
                message.setChatUnit(unit());
            append(message);
        }
    });
}

QUrl ChatChannel::appendTextUrl(const QString &text)
{
    return commandUrl(QStringLiteral("appendText"), text);
}

QUrl ChatChannel::appendNickUrl(const QString &nick)
{
    return commandUrl(QStringLiteral("appendNick"), nick);
}

QUrl ChatChannel::commandUrl(const QString &method, const QString &arg) const
{
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("id"), id());
    query.addQueryItem(QStringLiteral("method"), method);
    query.addQueryItem(QStringLiteral("arg"), arg);

    QUrl url;
    url.setScheme(QStringLiteral("session"));
    url.setQuery(query);

    return url;
}

qint64 ChatChannel::doAppendMessage(qutim_sdk_0_3::Message &message)
{
	if (message.isIncoming())
		emit messageReceived(&message);
	else
		emit messageSent(&message);
	
	if (message.property("spam", false) || message.property("hide", false))
		return message.id();
	
	bool service = message.property("service", false);
	
	if ((!isActive() && !service) && message.isIncoming()) {
		m_unread.append(message);
		emit unreadChanged(m_unread);
		emit unreadCountChanged(m_unread.count());
	}
	
	if (!message.property("silent", false) && !isActive())
		Notification::send(message);
	
	emit messageAppended(message);
	return message.id();
}

void ChatChannel::doSetActive(bool active)
{
	if (active)
		markRead(Q_UINT64_C(0xffffffffffffffff));
}

void ChatChannel::connectNotify(const QMetaMethod &signal)
{
    ChatSession::connectNotify(signal);
    if (signal == QMetaMethod::fromSignal(&ChatChannel::javaScriptRequest))
        ++m_javaScriptListeners;
}

void ChatChannel::disconnectNotify(const QMetaMethod &signal)
{
    ChatSession::disconnectNotify(signal);
    if (signal == QMetaMethod::fromSignal(&ChatChannel::javaScriptRequest))
        --m_javaScriptListeners;
}

}
