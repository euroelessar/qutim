/****************************************************************************
 *  chatlayerimpl.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "chatlayerimpl.h"
#include <libqutim/account.h>
#include "chatsessionimpl.h"
#include "chatwidget.h"
#include "modulemanagerimpl.h"
#include <QDebug>
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>

namespace AdiumChat
{
	static Core::CoreModuleHelper<ChatLayerImpl> chat_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "Webkit chat layer"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat realization, based on Adium chat styles")
	);

//	inline ActionGenerator *generate(const char *name)
//	{
//		return new ActionGenerator(Icon("mail-message-new"),
//								   LocalizedString("ChatLayer", name),
//								   ChatLayer::instance(), SLOT(onStartChat()));
//	}

	void init()
	{
		ActionGenerator *action;
		action = new ActionGenerator(Icon("mail-message-new"),
									 QT_TRANSLATE_NOOP("ChatLayer", "Send message"),
									 ChatLayer::instance(), SLOT(onStartChat()));
		MenuController::addAction<Contact>(action);
//		MenuController::addAction<Contact>(generate("1"), "1first");
//		MenuController::addAction<Contact>(generate("2"), "1first\0""1first");
//		MenuController::addAction<Contact>(generate("3"), "1first\0""2second");
//		MenuController::addAction<Contact>(generate("4"), "2second");
//		MenuController::addAction<Contact>(generate("5"), "2second\0""1first");
//		MenuController::addAction<Contact>(generate("6"), "3third");
	}

	static Core::CoreStartupHelper<&init> action_init_static(
		QT_TRANSLATE_NOOP("Plugin", "Helper for chat layer"),
		QT_TRANSLATE_NOOP("Plugin", "Adds \"Start chat\" action to conact's menu")
	);

	ChatLayerImpl::ChatLayerImpl()
	{
	}


	ChatSession* ChatLayerImpl::getSession(ChatUnit* unit, bool create)
	{
		//find or create session
 		if(!(unit = getUnitForSession(unit)))
 			return 0;
		ChatSessionImpl *session = m_chat_sessions.value(unit);
		if(!session && create)
		{
			session = new ChatSessionImpl(unit,this);
			connect(session,SIGNAL(activated(bool)),SLOT(onSessionActivated(bool)));
			m_chat_sessions.insert(unit,session);
			emit sessionCreated(session);
		}
		return session;
	}

	QList<ChatSession* > ChatLayerImpl::sessions()
	{
		QList<ChatSession* >  list;
		ChatSessionHash::const_iterator it;
		for (it=m_chat_sessions.constBegin();it!=m_chat_sessions.constEnd();it++)
			list.append(it.value());
// 		foreach (ChatSession *sess, m_chat_sessions)
// 			list.append(sess);
		return list;
	}

	void ChatLayerImpl::onChatWidgetDestroyed(QObject* object)
	{
		ChatWidget *widget = qobject_cast< ChatWidget* >(object);
		QString key = m_chatwidgets.key(widget);
		m_chatwidgets.remove(key);
	}

	ChatLayerImpl::~ChatLayerImpl()
	{

	}

	QString ChatLayerImpl::getWidgetId(ChatSessionImpl* sess) const
	{
		QString key;
		//QString key = acc->id() + "/" + id; //simple variant
		key = "adiumchat"; //all session in one window
		return key;
	}

	void ChatLayerImpl::onSessionActivated(bool active)
	{
		//init or update chat widget(s)
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		if (!session)
			return;
		QString key = getWidgetId(session);
		ChatWidget *widget = m_chatwidgets.value(key,0);
		if (!widget)
		{
			if (!active)
				return;
			widget = new ChatWidget(true);
			m_chatwidgets.insert(key,widget);
			connect(widget,SIGNAL(destroyed(QObject*)),SLOT(onChatWidgetDestroyed(QObject*)));
			widget->show();
		}
		if (active)
		{
			if (!widget->contains(session))
				widget->addSession(session);
			widget->activate(session);
		}
	}

	void ChatLayerImpl::onStartChat()
	{
		QAction *action = qobject_cast<QAction *>(sender());
		Q_ASSERT(action);
		QVariant data = action->data();
		MenuController *menu = action->data().value<MenuController *>();
		if (ChatUnit *unit = qobject_cast<ChatUnit *>(menu)) {
			if (ChatSession *session = getSession(unit, true))
				session->activate();
		}
	}

	ChatWidget *AdiumChat::ChatLayerImpl::findWidget(ChatSession *sess) const
	{
		ChatWidgetHash::const_iterator it;
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sess);
		for (it=m_chatwidgets.constBegin();it!=m_chatwidgets.constEnd();it++) {
			if (it.value()->contains(session))
				return it.value();
		}
		return 0;
	}

	QTextDocument *ChatLayerImpl::getInputField(ChatSession *session)
	{
		ChatWidget *widget = findWidget(session);
		return widget ? widget->getInputField() : 0;
	}

}

