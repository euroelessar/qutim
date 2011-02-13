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
#include <QDeclarativeContext>
#include <QDeclarativeEngine>

namespace Core {
namespace AdiumChat {

QVariant messageToVariant(const Message &mes)
{
	//TODO Optimize if posible
	QVariant unit = qVariantFromValue<ChatUnit*>(const_cast<ChatUnit *>(mes.chatUnit()));
	QVariantMap map;
	map.insert(QLatin1String("id"), mes.id());
	map.insert(QLatin1String("time"), mes.time());
	map.insert(QLatin1String("unit"), unit);
	map.insert(QLatin1String("in"), mes.isIncoming());
	map.insert(QLatin1String("text"), mes.text());

	foreach(const QByteArray &name, mes.dynamicPropertyNames())
		map.insert(QString::fromUtf8(name), mes.property(name));
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
	QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(obj);
	addItem(item);

	QDeclarativeContext *context = m_engine->contextForObject(obj);

	//context->setContextProperty(QLatin1String("controller"), this);

	m_engine->rootContext()->setContextProperty(QLatin1String("controller"), this);
}

QuickChatViewController::~QuickChatViewController()
{

}

} // namespace AdiumChat
} // namespace Core
