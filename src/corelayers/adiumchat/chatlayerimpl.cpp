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
#include <libqutim/debug.h>
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>
#include <libqutim/shortcut.h>
#include <libqutim/conference.h>

namespace AdiumChat
{
	static Core::CoreModuleHelper<ChatLayerImpl> chat_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "Webkit chat layer"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat implementation, based on Adium chat styles")
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
		
		Shortcut::registerSequence("chatSendMessage",
								   QT_TRANSLATE_NOOP("ChatLayer", "Send message"),
								   "ChatWidget",
								   QKeySequence("Ctrl+Return")
								   );
								   
		Shortcut::registerSequence("chatCloseSession",
								   QT_TRANSLATE_NOOP("ChatLayer", "Close chat"),
								   "ChatWidget",
								   QKeySequence(QKeySequence::Close)
								   );   
		Shortcut::registerSequence("chatNext",
								   QT_TRANSLATE_NOOP("ChatLayer", "Next chat"),
								   "ChatWidget",
								   QKeySequence(QKeySequence::NextChild)
								   );
		Shortcut::registerSequence("chatPrevious",
								   QT_TRANSLATE_NOOP("ChatLayer", "Previous chat"),
								   "ChatWidget",
								   QKeySequence(QKeySequence::PreviousChild)
								   );
		
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
		qRegisterMetaType<QWidgetList>("QWidgetList");
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
			connect(session,SIGNAL(destroyed(QObject*)),SLOT(onChatSessionDestroyed(QObject*)));
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
		ChatWidget *widget = reinterpret_cast < ChatWidget* >(object);
		QString key = m_chatwidgets.key(widget);
		m_chatwidgets.remove(key);
	}
	void ChatLayerImpl::onChatSessionDestroyed(QObject *object)
	{
		ChatSessionImpl *sess = reinterpret_cast<ChatSessionImpl *>(object);
		if (!sess)
			return;
		ChatUnit *key = m_chat_sessions.key(sess);
//		debug() << "session removed" << sess << "unit:" << key;
		if (key)
			m_chat_sessions.remove(key);
	}

	ChatLayerImpl::~ChatLayerImpl()
	{

	}

	QString ChatLayerImpl::getWidgetId(ChatSessionImpl* sess) const
	{
		Q_UNUSED(sess);
		QString key;

		//TODO add configuration

		if (qobject_cast<const Conference *>(sess->getUnit()))
			key = "conference";
		else
			key = "chat";

		return key;
	}

	void ChatLayerImpl::onSessionActivated(bool active)
	{
		debug() << "session activated";
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
//		QVariant data = action->data();
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

	QWidgetList ChatLayerImpl::chatWidgets()
	{
		QWidgetList list;
		foreach (QWidget *widget, m_chatwidgets)
			list << widget;
		return list;
	}

}

