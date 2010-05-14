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
#include "modulemanagerimpl.h"
#include <libqutim/debug.h>
#include <libqutim/settingslayer.h>
#include <libqutim/icon.h>
#include <libqutim/shortcut.h>
#include <libqutim/conference.h>

namespace Core
{
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
			return list;
		}

		void ChatLayerImpl::onChatSessionDestroyed(QObject *object)
		{
			ChatSessionImpl *sess = reinterpret_cast<ChatSessionImpl *>(object);
			ChatUnit *key = m_chat_sessions.key(sess);
			if (key)
				m_chat_sessions.remove(key);
		}

		ChatLayerImpl::~ChatLayerImpl()
		{
		}

		QIcon ChatLayerImpl::iconForState(ChatState state)
		{
			QString icon_name;
			switch (state) {
				//FIXME icon names
			case ChatStateActive:
				icon_name = "im-user";
				break;
			case ChatStateInActive:
				icon_name = "im-user-away";
				break;
			case ChatStateGone:
				icon_name =  "im-user-offline";
				break;
			case ChatStateComposing:
				icon_name = "im-status-message-edit";
				break;
			case ChatStatePaused:
				icon_name = "mail-unread";
				break;
			default:
				break;
			}
			return Icon(icon_name);
		}
	}
}
