#include "chatlayerimpl.h"
#include <libqutim/account.h>
#include "chatsessionimpl.h"
#include "chatwidget.h"
#include "modulemanagerimpl.h"
#include <QDebug>

namespace AdiumChat

{
	static Core::CoreModuleHelper<ChatLayerImpl> chat_layer_static(
		QT_TRANSLATE_NOOP("Plugin", "Webkit chat layer"),
		QT_TRANSLATE_NOOP("Plugin", "Default qutIM chat realization, based on Adium chat styles")
	);
		
	ChatLayerImpl::ChatLayerImpl()
	{

	}


	ChatSession* ChatLayerImpl::getSession ( Account* acc, const QString& id, bool create )
	{
		create = false;
		//find or create session
		if (m_chat_sessions.value(acc).contains(id) && !create)
			return m_chat_sessions.value(acc).value(id);
		ChatSessionImpl *session = new ChatSessionImpl(acc,id,this);
		if (!m_chat_sessions.contains(acc))
			connect(acc,SIGNAL(destroyed(QObject*)),SLOT(onAccountDestroyed(QObject*)));
		m_chat_sessions[acc].insert(id,session);
		//init or update chat widget(s)
		QString key = getWidgetId(acc,id);
		ChatWidget *widget = m_chatwidget_list.value(key);
		if (!widget)
		{
			widget = new ChatWidget();
			widget->show();
			m_chatwidget_list.insert(key,widget);//FIXME need testing
		}
		widget->addSession(session);
		
		connect(session,SIGNAL(removed(Account*,QString)),SLOT(onSessionRemoved(Account*,QString)));		
		return session;
	}


	QList<ChatSession* > ChatLayerImpl::sessions()
	{
		QList<ChatSession* >  list;
		foreach (ChatSessionHash hash, m_chat_sessions)
		{
			foreach (ChatSession *item, hash )
			{
				list.append(item);
			}
		}
		return list;
	}

	void ChatLayerImpl::onAccountDestroyed(QObject* object)
	{
		Account *acc = qobject_cast<Account *>(object);
 		if (!acc && !m_chat_sessions.contains(acc))
 			return;
 		qDeleteAll (m_chat_sessions[acc]);
 		m_chat_sessions.remove(acc);
	}

	void ChatLayerImpl::onSessionRemoved(Account* acc, const QString& id)
	{
		m_chat_sessions[acc].remove(id);
	}
	
	ChatLayerImpl::~ChatLayerImpl()
	{
		
	}
	
	QString ChatLayerImpl::getWidgetId(Account* acc, const QString& id) const
	{
		QString key;
		//QString key = acc->id() + "/" + id; //simple variant
		key = "adiumchat"; //all session in one window
		return key;
	}

}
