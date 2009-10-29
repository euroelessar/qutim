#include "chatsessionimpl.h"
#include "chatstyleoutput.h"
#include <QWebPage>
#include <QWebFrame>
#include <libqutim/account.h>
#include <QTextDocument>

namespace AdiumChat

{
	
	ChatSessionImpl::ChatSessionImpl (Contact *starter, ChatLayer* chat )
	: ChatSession (starter, chat ),m_chat_style_output(new ChatStyleOutput),m_web_page(new QWebPage), m_session_starter(starter)
	{
		m_chat_style_output->preparePage(m_web_page,m_session_starter);
		m_message_count = 0;
	}

	ChatSessionImpl::~ChatSessionImpl()
	{
		if (m_web_page)
		{
			m_web_page->deleteLater();
		}
	}

	void ChatSessionImpl::addContact ( Contact* c )
	{

	}

	void ChatSessionImpl::appendMessage ( const Message& message )
	{
		//TODO add normal check if contact is null
		if (!message.contact())
			return;
		bool same_from = false;
		bool isHistory = message.property("isHistory").toBool();
		if (isHistory)
		{
			m_previous_sender="";
		}
		QString item;
		if(message.text().startsWith("/me "))
		{
			QString tmp_msg = message.text().mid(3);
			item = m_chat_style_output->makeAction(
					Qt::escape(message.contact()->name()),
					tmp_msg, !message.isIncoming(), message.time(),
					message.contact()->property("imagepath").toString(),
					true, message.contact()->id(), message.contact()->account()->id());
			m_previous_sender = "";
		}
		else
		{
			same_from = (m_previous_sender == (message.isIncoming()?"nme":"me"));
			QString item = m_chat_style_output->makeMessage(message.contact()->name(),
															message.text(),
															message.isIncoming(),
															message.time(),
															message.contact()->property("imagepath").toString(),
															true, //FIXME
															message.contact()->id(),
															message.contact()->account()->id(),
															same_from,
															isHistory
															);
			m_previous_sender = (message.isIncoming()?"nme":"me");
		}

		QString result = m_web_page->mainFrame()->evaluateJavaScript(QString("getEditedHtml(\"%1\", \"%2\");")
																	 .arg(validateCpp(item))
																	 .arg(m_message_count)).toString();
		QString jsTask = QString("append%2Message(\"%1\");").arg(
				result.isEmpty() ? item :
				validateCpp(result.replace("\\","\\\\")), same_from?"Next":"");
		m_web_page->mainFrame()->evaluateJavaScript(jsTask);
		if (!result.isEmpty())
			m_message_count++;
	}

	void ChatSessionImpl::chatStateChanged ( Contact* c, ChatState state )
	{

	}

	void ChatSessionImpl::removeContact ( Contact* c )
	{

	}


}
