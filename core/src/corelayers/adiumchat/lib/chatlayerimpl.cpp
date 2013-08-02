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

#include "chatlayerimpl.h"
#include <qutim/account.h>
#include <qutim/metacontact.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/shortcut.h>
#include <qutim/conference.h>
#include <qutim/servicemanager.h>
#include <qutim/adiumchat/chatsessionimpl.h>
#include <qutim/adiumchat/conferencetabcompletion.h>
#include <qutim/adiumchat/abstractchatform.h>
#include <qutim/adiumchat/chatviewfactory.h>
#include <QPlainTextEdit>

namespace Core
{
namespace AdiumChat
{

void init()
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ChatForm"));
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ChatViewFactory"));
	Shortcut::registerSequence("chatCloseSession",
							   QT_TRANSLATE_NOOP("ChatLayer", "Close chat"),
							   QT_TRANSLATE_NOOP("ChatLayer", "ChatWidget"),
							   QKeySequence(QKeySequence::Close)
							   );
	Shortcut::registerSequence("chatNext",
							   QT_TRANSLATE_NOOP("ChatLayer", "Next chat"),
							   QT_TRANSLATE_NOOP("ChatLayer", "ChatWidget"),
							   QKeySequence(QKeySequence::NextChild)
							   );
	Shortcut::registerSequence("chatPrevious",
							   QT_TRANSLATE_NOOP("ChatLayer", "Previous chat"),
							   QT_TRANSLATE_NOOP("ChatLayer", "ChatWidget"),
							   QKeySequence(QKeySequence::PreviousChild)
							   );
}

ChatLayerImpl::ChatLayerImpl()
{
	qRegisterMetaType<QWidgetList>("QWidgetList");
	init();
	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QObject*,QObject*)),
			SLOT(onServiceChanged(QObject*)));
}


ChatSession* ChatLayerImpl::getSession(ChatUnit* unit, bool create)
{
	// TODO: Think, is it good idea or we need smth more intellegent?
	if (ChatUnit *meta = unit->metaContact())
		unit = meta;
	//find or create session
	if(!(unit = getUnitForSession(unit)))
		return 0;
	ChatSessionImpl *session = m_chatSessions.value(unit);
	if(!session && create) {
		session = new ChatSessionImpl(unit, this);
		connect(session, SIGNAL(destroyed(QObject*)), SLOT(onChatSessionDestroyed(QObject*)));
		m_chatSessions.insert(unit,session);
		emit sessionCreated(session);
		connect(session, SIGNAL(activated(bool)), SLOT(onChatSessionActivated(bool)));
	}
	return session;
}

QList<ChatSession* > ChatLayerImpl::sessions()
{
	QList<ChatSession* >  list;
	ChatSessionHash::const_iterator it;
	for (it=m_chatSessions.constBegin();it!=m_chatSessions.constEnd();it++)
		list.append(it.value());
	return list;
}

void ChatLayerImpl::onChatSessionDestroyed(QObject *object)
{
	ChatSessionImpl *sess = reinterpret_cast<ChatSessionImpl *>(object);
	ChatUnit *key = m_chatSessions.key(sess);
	if (key)
		m_chatSessions.remove(key);
}

ChatLayerImpl::~ChatLayerImpl()
{
}

QIcon ChatLayerImpl::iconForState(ChatState state, const ChatUnit *unit)
{
	if (state != ChatStateComposing) {
		QVariant status = unit->property("status");
		if (!status.isNull() && status.canConvert<Status>()) {
			return status.value<Status>().icon();
		}
	}
	if (qobject_cast<const Conference*>(unit))
		return Icon("view-conversation-balloon");

	QString iconName;
	switch (state) {
	//FIXME icon names
	case ChatStateActive:
		iconName = "im-user"; //TODO find icon
		break;
	case ChatStateInActive:
		iconName = "im-user-away";
		break;
	case ChatStateGone:
		iconName =  "im-user-offline";
		break;
	case ChatStateComposing:
		iconName = "im-status-message-edit";
		break;
	case ChatStatePaused:
		iconName = "im-user-busy";
		break;
	default:
		break;
	}
	return Icon(iconName);
}

void ChatLayerImpl::insertText(ChatSession *session, const QString &text, bool setFocus)
{
	AbstractChatForm *form = ServiceManager::getByName<AbstractChatForm*>("ChatForm");
	QObject *obj = form->textEdit(session);
	QTextCursor cursor;
	if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj))
		cursor = edit->textCursor();
	else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj))
		cursor = edit->textCursor();
	else
		return;
	cursor.insertText(text);
	if (setFocus)
		static_cast<QWidget*>(obj)->setFocus();
}

void ChatLayerImpl::onChatSessionActivated(bool activated)
{
	if (!activated)
		return;

	ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
	Q_ASSERT(session);
	qDebug() << "activate session" << session->unit()->title();

	if (qobject_cast<Conference*>(session->getUnit())) {
		QObject *form = ServiceManager::getByName("ChatForm");
		QObject *obj = 0;
		if (QMetaObject::invokeMethod(form, "textEdit", Q_RETURN_ARG(QObject*, obj),
									  Q_ARG(qutim_sdk_0_3::ChatSession*, session)) && obj) {
			if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj)) {
				if (m_tabCompletion.isNull()) {
					m_tabCompletion = new ConfTabCompletion(this);
				}
				m_tabCompletion.data()->setTextEdit(edit);
				m_tabCompletion.data()->setChatSession(session);
				return;
			}
		}
	}
	else if (!m_tabCompletion.isNull()) {
		m_tabCompletion.data()->deleteLater();
	}
}

void ChatLayerImpl::onServiceChanged(QObject *now)
{
	if (qobject_cast<ChatViewFactory*>(now)) {
		foreach (const ChatSessionImpl *session, m_chatSessions)
			if (session && session->controller()) {
				session->controller()->deleteLater();
			}
	}
}

}
}

#include "moc_chatlayerimpl.cpp"
