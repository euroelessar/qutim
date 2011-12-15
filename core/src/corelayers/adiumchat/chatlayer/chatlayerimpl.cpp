/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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
#include "chatsessionimpl.h"
#include "conferencetabcompletion.h"
#include "chatforms/abstractchatform.h"
#include <QPlainTextEdit>

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
	if (state != ChatStateComposing) {
		QVariant status = unit->property("status");
		if (!status.isNull() && status.canConvert<Status>()) {
			return status.value<Status>().icon();
		}
	}
	if (qobject_cast<const Conference*>(unit))
		return Icon("view-conversation-balloon");

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

void ChatLayerImpl::insertText(ChatSessionImpl *session, const QString &text, bool setFocus)
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

