/****************************************************************************
 *  quickchatviewcontroller.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "quickchatviewcontroller.h"
#include <qutim/message.h>
#include <chatlayer/chatsessionimpl.h>
#include <qutim/thememanager.h>
#include <QDeclarativeComponent>
#include <QStringBuilder>
#include <QDeclarativeItem>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <qutim/conference.h>

namespace Core {
namespace AdiumChat {

QVariant messageToVariant(const Message &mes)
{
	//TODO Optimize if posible
	QVariantMap map;
	map.insert(QLatin1String("mid"), mes.id());
	map.insert(QLatin1String("time"), mes.time());
	QObject *unit = const_cast<ChatUnit*>(mes.chatUnit());
	if (unit) {
		map.insert(QLatin1String("chatUnit"), qVariantFromValue<QObject*>(unit));
		QObject *account = const_cast<Account*>(mes.chatUnit()->account());
		map.insert(QLatin1String("account"), qVariantFromValue<QObject*>(account));
	}
	map.insert(QLatin1String("isIncoming"), mes.isIncoming());
	map.insert(QLatin1String("isDelivered"), mes.isIncoming());
	map.insert(QLatin1String("body"), mes.text());

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

	return map;
}

QuickChatViewController::QuickChatViewController(/*QDeclarativeEngine *engine*/) :
	m_session(0),
	m_themeName(QLatin1String("default")),
//	m_engine(engine) //TODO use one engine for all controllers
	m_engine(new QDeclarativeEngine(this))
{

}

	
void QuickChatViewController::appendMessage(const qutim_sdk_0_3::Message& msg)
{
	emit messageAppended(messageToVariant(msg));
}

void QuickChatViewController::clearChat()
{
	
}

ChatSessionImpl* QuickChatViewController::getSession() const
{
	return m_session;
}

void QuickChatViewController::setChatSession(ChatSessionImpl* session)
{
	if (m_session == session)
		return;
	
	if(m_session) {
		m_session->disconnect(this);
		m_session->removeEventFilter(this);
	}
	m_session = session;
	m_session->installEventFilter(this);
	
	QString path = ThemeManager::path(QLatin1String("qmlchat"), m_themeName);
	QString main = path % QLatin1Literal("/main.qml");

	QGraphicsScene::clear();
	QDeclarativeComponent component(m_engine, main);
	QObject *obj = component.create();
	m_item = qobject_cast<QDeclarativeItem*>(obj);
	addItem(m_item);

	//QDeclarativeContext *context = m_engine->contextForObject(obj);

	//context->setContextProperty(QLatin1String("controller"), this);

	m_engine->rootContext()->setContextProperty(QLatin1String("controller"), this);
}

QuickChatViewController::~QuickChatViewController()
{

}

QDeclarativeItem *QuickChatViewController::rootItem() const
{
	return m_item;
}

bool QuickChatViewController::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == MessageReceiptEvent::eventType()) {
		MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
		emit messageDelivered(msgEvent->id());
		return true;
	}
	return QGraphicsScene::eventFilter(obj, ev);
}

} // namespace AdiumChat
} // namespace Core
