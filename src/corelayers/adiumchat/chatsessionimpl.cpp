#include "chatsessionimpl.h"
#include "chatstyleoutput.h"
#include <QWebPage>
#include <QWebFrame>
#include <libqutim/account.h>
#include <QTextDocument>
#include <QDateTime>
#include <QDebug>

namespace AdiumChat

{
	
	ChatSessionImpl::ChatSessionImpl ( Account* acc, const QString& id, ChatLayer* chat)
	: ChatSession ( chat ),m_chat_style_output(new ChatStyleOutput),m_web_page(new QWebPage)
	{
		m_account = acc;
		m_session_id = id;
		m_chat_style_output->preparePage(m_web_page,m_account,m_session_id);
		m_message_count = 0;
	}

	ChatSessionImpl::~ChatSessionImpl()
	{
		if (m_web_page)
		{
			m_web_page->deleteLater();
		}
		emit removed(m_account,m_session_id);
		qDebug() << "Session removed:" << m_account->id() << m_session_id;
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
		bool isHistory = message.time().isValid();
		if (isHistory)
		{
			m_previous_sender="";
		}
		QString item;
		if(message.text().startsWith("/me "))
		{
			QString tmp_msg = message.text().mid(3);
			qDebug() << tmp_msg;
			item = m_chat_style_output->makeAction(
					Qt::escape(message.contact()->name()),
					tmp_msg, !message.isIncoming(), message.time(),
					message.contact()->property("imagepath").toString(),
					true, message.contact()->id(), message.contact()->account()->id());
			m_previous_sender = "";
		}
		else if (message.property("service").toBool())
		{
			item = m_chat_style_output->makeStatus(item, QDateTime::currentDateTime());
			m_previous_sender = "";
		}
		else
		{
			same_from = (m_previous_sender == (message.isIncoming()?"nme":"me"));
			item = m_chat_style_output->makeMessage(this, message, true, //FIXME
															same_from );
			m_previous_sender = (message.isIncoming()?"nme":"me");
		}

		QString result = m_web_page->mainFrame()->evaluateJavaScript(QString("getEditedHtml(\"%1\", \"%2\");")
																	 .arg(validateCpp(item))
																	 .arg(m_message_count)).toString();
		QString jsTask = QString("append%2Message(\"%1\");").arg(
				result.isEmpty() ? item :
				validateCpp(result.replace("\\","\\\\")), same_from?"Next":"");
		qDebug() << jsTask << item;
		m_web_page->mainFrame()->evaluateJavaScript(jsTask);
		if (result.isEmpty()) //TODO I'm not sure that it works well
			m_message_count++;
	}

	void ChatSessionImpl::chatStateChanged ( Contact* c, ChatState state )
	{

	}

	void ChatSessionImpl::removeContact ( Contact* c )
	{

	}

	
	QWebPage* ChatSessionImpl::getPage() const
	{
		return m_web_page;
	}

	Account* ChatSessionImpl::getAccount() const
	{
		return m_account;
	}

	QString ChatSessionImpl::getId() const
	{
		return m_session_id;
	}

	QVariant ChatSessionImpl::evaluateJavaScript(const QString &scriptSource)
	{
		if(m_web_page.isNull())
			return QVariant();
		return m_web_page->mainFrame()->evaluateJavaScript(scriptSource);
	}
}
