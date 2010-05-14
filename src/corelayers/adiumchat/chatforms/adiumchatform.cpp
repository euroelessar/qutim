#include "adiumchatform.h"
#include "modulemanagerimpl.h"
#include "adiumchatwidget.h"
#include <libqutim/configbase.h>
#include <libqutim/conference.h>
#include <libqutim/debug.h>
#include "../chatsessionimpl.h"

namespace Core
{
	namespace AdiumChat
	{
		static CoreModuleHelper<AdiumChatForm> classic_chatform_static(
				QT_TRANSLATE_NOOP("Plugin", "Adium"),
				QT_TRANSLATE_NOOP("Plugin", "Adium-like chatform implementation for adiumchat")
				);
		
		AdiumChatForm::AdiumChatForm()
		{
			connect(ChatLayerImpl::instance(),SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*))
					);
		}

		void AdiumChatForm::onChatWidgetDestroyed(QObject* object)
		{
			AdiumChatWidget *widget = reinterpret_cast<AdiumChatWidget*>(object);
			QString key = m_chatwidgets.key(widget);
			m_chatwidgets.remove(key);
		}


		QString AdiumChatForm::getWidgetId(ChatSessionImpl* sess) const
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
		
		void AdiumChatForm::onSessionActivated(bool active)
		{
			debug() << "session activated";
			//init or update chat widget(s)
			ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
			if (!session)
				return;
			QString key = getWidgetId(session);
			AdiumChatWidget *widget = m_chatwidgets.value(key,0);
			if (!widget)
			{
				if (!active)
					return;
				widget = new AdiumChatWidget(true);
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

		AdiumChatWidget *AdiumChatForm::findWidget(ChatSession *sess) const
		{
			ChatWidgetHash::const_iterator it;
			ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sess);
			for (it=m_chatwidgets.constBegin();it!=m_chatwidgets.constEnd();it++) {
				if (it.value()->contains(session))
					return it.value();
			}
			return 0;
		}
		

		QWidgetList AdiumChatForm::chatWidgets()
		{
			QWidgetList list;
			foreach (QWidget *widget, m_chatwidgets)
				list << widget;
			return list;
		}
		
		void AdiumChatForm::onSessionCreated(ChatSession* session)
		{
			debug() << "Chatform: session created";
			connect(session,SIGNAL(activated(bool)),SLOT(onSessionActivated(bool)));
		}

		AdiumChatForm::~AdiumChatForm()
		{
			qDeleteAll(m_chatwidgets);
		}

	}
}
