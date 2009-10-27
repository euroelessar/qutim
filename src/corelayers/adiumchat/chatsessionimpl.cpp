#include "chatsessionimpl.h"
#include "chatstyleoutput.h"
#include <QWebPage>
#include <QWebFrame>
#include <libqutim/account.h>

namespace AdiumChat

{
	
	ChatSessionImpl::ChatSessionImpl ( ChatLayer* chat )
	: ChatSession ( chat ),m_chat_style_output(new ChatStyleOutput),m_web_page(new QWebPage)
	{
		m_chat_style_output->preparePage(m_web_page);
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
		//TODO
		bool same_from = false;
		QString item = m_chat_style_output->makeMessage(message.contact()->name(),
														message.text(),
														message.isIncoming(),
														message.time(),
														message.contact()->property("imagepath").toString(),
														true, //FIXME
														message.contact()->id(),
														message.contact()->account()->id(),
														same_from,
														message.property("isHistory").toBool()
														);
		QString result; //TODO
		QString jsTask = QString("append%2Message(\"%1\");").arg(
				result.isEmpty() ? item :
				validateCpp(result.replace("\\","\\\\")), same_from?"Next":"");
		m_web_page->mainFrame()->evaluateJavaScript(jsTask);

	}

	void ChatSessionImpl::chatStateChanged ( Contact* c, ChatState state )
	{

	}

	void ChatSessionImpl::removeContact ( Contact* c )
	{

	}


}