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

#include "quickchatviewcontroller.h"
#include <qutim/message.h>
#include <qutim/thememanager.h>
#include <QQmlComponent>
#include <QStringBuilder>
#include <QQuickItem>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlProperty>
#include <qutim/conference.h>
#include <qutim/history.h>
#include <qutim/emoticons.h>
#include <qutim/notification.h>
#include <qutim/declarativeview.h>

#include <QImageReader>
#include <qutim/servicemanager.h>
#include <qutim/utils.h>
#include <QPlainTextEdit>

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

static QVariant messageToVariant(const Message &mes)
{
	//TODO Optimize if posible
	QVariantMap map;

    map.insert(QStringLiteral("originalMessage"), QVariant::fromValue(mes));
	map.insert(QStringLiteral("messageId"), mes.id());
	map.insert(QStringLiteral("time"), mes.time().isValid() ? mes.time() : QDateTime::currentDateTime());

	if (QObject *unit = mes.chatUnit()) {
		map.insert(QStringLiteral("chatUnit"), qVariantFromValue<QObject*>(unit));
		QObject *account = mes.chatUnit()->account();
		map.insert(QStringLiteral("account"), qVariantFromValue<QObject*>(account));
	}

	map.insert(QStringLiteral("incoming"), mes.isIncoming());
	map.insert(QStringLiteral("delivered"), mes.isIncoming());

	//define action and service property if it not defined
	map.insert(QStringLiteral("action"), mes.property("action",false));
	map.insert(QStringLiteral("service"), mes.property("service",false));

	//handle /me
	bool isMe = mes.text().startsWith(QStringLiteral("/me "));
	QString body = isMe ? mes.text().mid(4) : mes.text();
	if (isMe)
		map.insert(QStringLiteral("action"), true);
	map.insert(QStringLiteral("body"), UrlParser::parseUrls(body));
    map.insert(QStringLiteral("text"), mes.text());
    map.insert(QStringLiteral("html"), mes.html());

	foreach(const QByteArray &name, mes.dynamicPropertyNames())
		map.insert(QString::fromUtf8(name), mes.property(name));

    const Message::UnitData unit = mes.unitData();
    map.insert(QStringLiteral("senderId"), unit.title);
    map.insert(QStringLiteral("senderName"), unit.id);
    map.insert(QStringLiteral("senderAvatar"), unit.avatar);

	return map;
}

QuickChatController::QuickChatController(QObject *parent) :
	QObject(parent)
{
}

QuickChatController::~QuickChatController()
{
}

void QuickChatController::appendMessage(const qutim_sdk_0_3::Message& msg)
{
	if (msg.text().isEmpty())
		return;
	emit messageAppended(messageToVariant(msg));
}

void QuickChatController::clearChat()
{
	emit clearChatField();
}

ChatSession *QuickChatController::getSession() const
{
	return m_session.data();
}

void QuickChatController::loadHistory()
{
	qDebug() << Q_FUNC_INFO;
	Config config = Config(QStringLiteral("appearance")).group(QStringLiteral("chat/history"));
	int max_num = 50 + config.value(QStringLiteral("maxDisplayMessages"), 5);
	MessageList messages = History::instance()->read(m_session.data()->getUnit(), max_num);
	foreach (Message mess, messages) {
		mess.setProperty("silent", true);
		mess.setProperty("store", false);
		mess.setProperty("history", true);
		if (!mess.chatUnit()) //TODO FIXME
			mess.setChatUnit(m_session.data()->getUnit());
		m_session.data()->append(mess);
	}
}

void QuickChatController::setChatSession(ChatSession *session)
{
	if (m_session.data() == session)
		return;

	if(m_session) {
		m_session.data()->disconnect(this);
		m_session.data()->removeEventFilter(this);
	} else
		m_session = session;
	m_session.data()->installEventFilter(this);
	loadSettings();
	emit sessionChanged(session);
}

bool QuickChatController::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		emit messageDelivered(msgEvent->id());
		return true;
	}
	return QObject::eventFilter(obj, ev);
}

void QuickChatController::loadSettings()
{
	Config config(QStringLiteral("appearance/quickChat"));
    config.beginGroup(QStringLiteral("style"));
	loadTheme(config.value(QStringLiteral("name"), QStringLiteral("default")));
}

void QuickChatController::loadTheme(const QString &name)
{
    QQuickItem *item = m_item;

	QString path = ThemeManager::path(QStringLiteral("qmlchat"), name);
	QString main = path % QLatin1Literal("/main.qml");

	QQmlComponent component (DeclarativeView::globalEngine(), QUrl::fromLocalFile(main));
	QObject *obj = component.create();
    if (!obj) {
        qDebug() << component.errors();
        return;
    }

	m_item = qobject_cast<QQuickItem*>(obj);
    QQmlEngine::setObjectOwnership(m_item, QQmlEngine::CppOwnership);

    QQmlProperty controllerProperty(m_item, QStringLiteral("controller"));
    controllerProperty.write(QVariant::fromValue(this));

    emit itemChanged(m_item);

    delete item;

	loadHistory();
}

QString QuickChatController::parseEmoticons(const QString &text) const
{
	//TODO Write flexible textfield with animated emoticons, copy/paste and follow links support
    return Emoticons::theme().parseEmoticons(text);
}

QQuickItem *QuickChatController::item() const
{
    return m_item;
}

void QuickChatController::appendText(const QString &text)
{
    qDebug() << Q_FUNC_INFO << text << m_session.data();
	QMetaObject::invokeMethod(ChatLayer::instance(),
	                          "insertText",
	                          Q_ARG(ChatSession*, m_session.data()),
	                          Q_ARG(QString, text + QStringLiteral(" ")));
}

} // namespace AdiumChat
} // namespace Core

