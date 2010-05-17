#include "classicchatform.h"
#include "modulemanagerimpl.h"
#include "classicchatwidget.h"
#include <libqutim/configbase.h>
#include <libqutim/conference.h>
#include <libqutim/debug.h>
#include "../chatsessionimpl.h"
#include <QPlainTextEdit>

namespace Core
{
	namespace AdiumChat
	{
		static CoreModuleHelper<ClassicChatForm> classic_chatform_static(
				QT_TRANSLATE_NOOP("Plugin", "Classic"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM chatform implementation for adiumchat")
				);
		
		ClassicChatForm::ClassicChatForm()
		{
			connect(ChatLayerImpl::instance(),SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*))
					);
		}

		void ClassicChatForm::onChatWidgetDestroyed(QObject* object)
		{
			ClassicChatWidget *widget = reinterpret_cast<ClassicChatWidget*>(object);
			QString key = m_chatwidgets.key(widget);
			m_chatwidgets.remove(key);
		}


		QString ClassicChatForm::getWidgetId(ChatSessionImpl* sess) const
		{
			QString key;

			ConfigGroup group = Config("behavior/chat").group("widget");
			int windows = group.value<int>("windows",0);

			//TODO add configuration

			if (!windows)
				key = "session";
			else {
				if (qobject_cast<const Conference*>(sess->getUnit()))
					key = "conference";
				else
					key = "chat";
			}

			return key;
		}
		
		void ClassicChatForm::onSessionActivated(bool active)
		{
			debug() << "session activated";
			//init or update chat widget(s)
			ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
			if (!session)
				return;
			QString key = getWidgetId(session);
			ClassicChatWidget *widget = m_chatwidgets.value(key,0);
			if (!widget)
			{
				if (!active)
					return;
				widget = new ClassicChatWidget(true);
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

		ClassicChatWidget *ClassicChatForm::findWidget(ChatSession *sess) const
		{
			ChatWidgetHash::const_iterator it;
			ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sess);
			for (it=m_chatwidgets.constBegin();it!=m_chatwidgets.constEnd();it++) {
				if (it.value()->contains(session))
					return it.value();
			}
			return 0;
		}

		QObject *ClassicChatForm::textEdit(ChatSession *session)
		{
			ClassicChatWidget *widget = findWidget(session);
			if (widget && widget->currentSession() == session)
				return widget->getInputField();
			return 0;
		}

		QWidgetList ClassicChatForm::chatWidgets()
		{
			QWidgetList list;
			foreach (QWidget *widget, m_chatwidgets)
				list << widget;
			return list;
		}
		
		void ClassicChatForm::onSessionCreated(ChatSession* session)
		{
			debug() << "Chatform: session created";
			connect(session,SIGNAL(activated(bool)),SLOT(onSessionActivated(bool)));
		}

		ClassicChatForm::~ClassicChatForm()
		{
			qDeleteAll(m_chatwidgets);
		}

	}
}
