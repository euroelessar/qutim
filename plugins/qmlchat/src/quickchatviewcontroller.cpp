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
	map.insert(QLatin1String("mid"), mes.id());
	map.insert(QLatin1String("time"), mes.time().isValid() ? mes.time() : QDateTime::currentDateTime());
	QObject *unit = mes.chatUnit();
	if (unit) {
		map.insert(QLatin1String("chatUnit"), qVariantFromValue<QObject*>(unit));
		QObject *account = mes.chatUnit()->account();
		map.insert(QLatin1String("account"), qVariantFromValue<QObject*>(account));
	}
	map.insert(QLatin1String("isIncoming"), mes.isIncoming());
	map.insert(QLatin1String("isDelivered"), mes.isIncoming());

	//define action and service property if it not defined
	map.insert(QLatin1String("action"), mes.property("action",false));
	map.insert(QLatin1String("service"), mes.property("service",false));

	//handle /me
	bool isMe = mes.text().startsWith(QLatin1String("/me "));
	QString body = isMe ? mes.text().mid(4) : mes.text();
	if (isMe)
		map.insert(QLatin1String("action"), true);
	map.insert(QLatin1String("body"), UrlParser::parseUrls(body));

	foreach(const QByteArray &name, mes.dynamicPropertyNames())
		map.insert(QString::fromUtf8(name), mes.property(name));

	//add correct sender name
	QString sender = mes.property("senderName", QString());
	if (sender.isEmpty()) {
		if (mes.isIncoming())
			map.insert(QLatin1String("sender"), mes.chatUnit()->title());
		else {
			Conference *c = qobject_cast<Conference*>(mes.chatUnit());
			if (c && c->me())
				map.insert(QLatin1String("sender"), c->me()->title());
			else
				map.insert(QLatin1String("sender"), mes.chatUnit()->account()->name());
		}
	} else
		map.insert(QLatin1String("sender"), sender);
	//add correct avatar
	if (!mes.property("avatar").isValid()) {
		if (mes.isIncoming()) {
			if (Buddy *b = qobject_cast<Buddy*>(mes.chatUnit()))
				map.insert(QLatin1String("avatar"), b->avatar());
		}
	}

	return map;
}

static QString chatStateToString(ChatState state)
{
	QString stateStr;
	switch (state) {
	case ChatStateActive:
		stateStr = QLatin1String("ChatStateActive");
		break;
	case ChatStateInActive:
		stateStr = QLatin1String("ChatStateInActive");
		break;
	case ChatStateGone:
		stateStr = QLatin1String("ChatStateGone");
		break;
	case ChatStateComposing:
		stateStr = QLatin1String("ChatStateComposing");
		break;
	case ChatStatePaused:
		stateStr = QLatin1String("ChatStatePaused");
		break;
	};
	return stateStr;
}

QuickChatController::QuickChatController(QObject *parent) :
	QObject(parent),
	m_themeName(QLatin1String("default"))
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
	Config config = Config(QLatin1String("appearance")).group(QLatin1String("chat/history"));
	int max_num = config.value(QLatin1String("maxDisplayMessages"), 5);
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

	connect(session->unit(), SIGNAL(chatStateChanged(qutim_sdk_0_3::ChatState,qutim_sdk_0_3::ChatState)),
			this, SLOT(onChatStateChanged(qutim_sdk_0_3::ChatState)));
}

QQuickItem *QuickChatController::rootItem() const
{
	return m_item.data();
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
	Config config("appearance/quickChat");
    config.beginGroup("style");
	loadTheme(config.value<QString>("name","default"));
}

void QuickChatController::loadTheme(const QString &name)
{
	m_themeName = name;
	QString path = ThemeManager::path(QLatin1String("qmlchat"), m_themeName);
	QString main = path % QLatin1Literal("/main.qml");

	QQmlComponent component (DeclarativeView::globalEngine(), QUrl::fromLocalFile(main));
	QObject *obj = component.create();
    if (!obj) {
        qDebug() << component.errors();
        return;
    }

	m_item = qobject_cast<QQuickItem*>(obj);
    QQmlProperty controllerProperty(m_item, QStringLiteral("controller"));
    controllerProperty.write(QVariant::fromValue(this));

	loadHistory();
}

QString QuickChatController::parseEmoticons(const QString &text) const
{
	//TODO Write flexible textfield with animated emoticons, copy/paste and follow links support
    return Emoticons::theme().parseEmoticons(text);
}

QObject *QuickChatController::unit() const
{
	return m_session ? m_session.data()->unit() : 0;
}

QString QuickChatController::chatState() const
{
	return chatStateToString(m_session ? m_session.data()->unit()->chatState() : ChatStateGone);
}

void QuickChatController::onChatStateChanged(qutim_sdk_0_3::ChatState state)
{
	emit chatStateChanged(chatStateToString(state));
}

void QuickChatController::appendText(const QString &text)
{
	qDebug() << Q_FUNC_INFO << text << m_session.data();
	QMetaObject::invokeMethod(ChatLayer::instance(),
	                          "insertText",
	                          Q_ARG(ChatSession*, m_session.data()),
	                          Q_ARG(QString, text + QLatin1String(" ")));
}

} // namespace AdiumChat
} // namespace Core

