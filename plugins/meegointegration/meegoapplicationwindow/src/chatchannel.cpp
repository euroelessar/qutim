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
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QTextDocument>
#include <qutim/thememanager.h>
#include <qutim/conference.h>
#include <qutim/notification.h>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QDateTime>
#include <QGraphicsObject>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

ChatController::ChatController()
	: WebKitMessageViewController(false), m_webView(0)
{
}

ChatController::~ChatController()
{
}

QString ChatController::fontFamily() const
{
	return m_fontFamily;
}

int ChatController::fontSize() const
{
	return m_fontSize;
}

QObject *ChatController::webView() const
{
	return m_webView;
}

void ChatController::setWebView(QObject *webView)
{
	if (m_webView != webView) {
		m_webView = webView;
		emit webViewChanged(webView);
		QString script = QLatin1String("client.handleElement(document.querySelector('*'));");
        QMetaObject::invokeMethod(m_webView, "evaluateJavaScript",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, script));
	}
}

void ChatController::fixFlickable(QObject *object)
{
	QGraphicsObject *graphicsObject = qobject_cast<QGraphicsObject*>(object);
	graphicsObject->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
}

void ChatController::append(const Message &message)
{
	appendMessage(message);
}

void ChatController::handleElement(const QWebElement &element)
{
	setPage(element.webFrame()->page());
}

void ChatController::appendNick(const QVariant &nick)
{
	emit nickAppended(nick.toString());
}

void ChatController::appendText(const QVariant &text)
{
	emit textAppended(text.toString());
}

void ChatController::setDefaultFont(const QString &family, int size)
{
	QFontInfo info(QFont(family, size));
	size = info.pixelSize();
	if (m_fontFamily != family) {
		m_fontFamily = family;
		emit fontFamilyChanged(family);
	}
	if (m_fontSize != size) {
		m_fontSize = size;
		emit fontSizeChanged(size);
	}
}

ChatChannel::ChatChannel(qutim_sdk_0_3::ChatUnit *unit)
	: ChatSession(Chat::instance()), m_unit(unit), m_page(0)
{
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

qint64 ChatChannel::send(const QString &text)
{    
	Message message(text);
	message.setIncoming(false);
	message.setChatUnit(m_unit);
	message.setTime(QDateTime::currentDateTime());
	return appendMessage(message);
}

void ChatChannel::showChat()
{
	static_cast<Chat*>(Chat::instance())->show();
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
	return true;
}

QVariant ChatChannel::evaluateJavaScript(const QString &script)
{
	QVariant result;
	emit javaScriptRequest(script, &result);
	return result;
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
}

