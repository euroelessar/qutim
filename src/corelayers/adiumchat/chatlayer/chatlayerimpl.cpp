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
#include <qutim/account.h>
#include <qutim/metacontact.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/shortcut.h>
#include <qutim/conference.h>
#include <qutim/servicemanager.h>
#include "chatsessionimpl.h"
#include "conferencetabcompletion.h"
#include <QElapsedTimer>

namespace Core
{
namespace AdiumChat
{
//	inline ActionGenerator *generate(const char *name)
//	{
//		return new ActionGenerator(Icon("mail-message-new"),
//								   LocalizedString("ChatLayer", name),
//								   ChatLayer::instance(), SLOT(onStartChat()));
//	}

void init()
{
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

ChatLayerImpl::ChatLayerImpl()
{
	qRegisterMetaType<QWidgetList>("QWidgetList");
	init();
}


ChatSession* ChatLayerImpl::getSession(ChatUnit* unit, bool create)
{
	// TODO: Think, is it good idea or we need smth more intellegent?
	if (ChatUnit *meta = unit->metaContact())
		unit = meta;
	//find or create session
	if(!(unit = getUnitForSession(unit)))
		return 0;
	ChatSessionImpl *session = m_chat_sessions.value(unit);
	if(!session && create) {
		session = new ChatSessionImpl(unit,this);
		connect(session,SIGNAL(destroyed(QObject*)),SLOT(onChatSessionDestroyed(QObject*)));
		m_chat_sessions.insert(unit,session);
		emit sessionCreated(session);
		connect(session,SIGNAL(activated(bool)),SLOT(onChatSessionActivated(bool)));
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

QIcon ChatLayerImpl::iconForState(ChatState state, const ChatUnit *unit)
{
	Q_UNUSED(unit);
	QString icon_name;
	switch (state) {
	//FIXME icon names
	case ChatStateActive:
		icon_name = "im-user"; //TODO find icon
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
		icon_name = "im-user-busy";
		break;
	default:
		break;
	}
	return Icon(icon_name);
}

struct MethodLiveCounter
{
	MethodLiveCounter(const char *m) : method(m) { timer.start(); }
	~MethodLiveCounter() { qDebug("%s %lld", method, timer.elapsed());}
	QElapsedTimer timer;
	const char *method;
};

void ChatLayerImpl::onChatSessionActivated(bool activated)
{
	MethodLiveCounter counter(Q_FUNC_INFO);
	if (!activated)
		return;

	ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
	Q_ASSERT(session);
	debug() << "activate session" << session->unit()->title();

	if (qobject_cast<Conference*>(session->getUnit())) {
		QObject *form = ServiceManager::getByName("ChatForm");
		QObject *obj = 0;
		if (QMetaObject::invokeMethod(form, "textEdit", Q_RETURN_ARG(QObject*, obj),
									  Q_ARG(qutim_sdk_0_3::ChatSession*, session)) && obj) {
			if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj)) {
				if (m_tab_completion.isNull()) {
					m_tab_completion = new ConfTabCompletion(this);
				}
				debug() << "Set conftabcompletion to" << session->unit()->title();
				m_tab_completion->setTextEdit(edit);
				m_tab_completion->setChatSession(session);
				return;
			}
		}
	}
	else if (!m_tab_completion.isNull()) {
		m_tab_completion->deleteLater();
	}

}
}
}
