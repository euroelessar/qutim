/****************************************************************************
 *  chatsessionimpl.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "chatsessionimpl.h"
#include "chatstyleoutput.h"
#include <QWebPage>
#include <QWebFrame>
#include <libqutim/account.h>
#include <QTextDocument>
#include <QDateTime>
#include <QDebug>
#include "libqutim/history.h"
#include <libqutim/notificationslayer.h>

namespace AdiumChat

{

	ChatSessionImpl::ChatSessionImpl ( Account* acc, const QString& id, ChatLayer* chat)
	: ChatSession ( chat ),m_chat_style_output(new ChatStyleOutput),m_web_page(new QWebPage)
	{
		m_account = acc;
		m_session_id = id;
		m_chat_style_output->preparePage(m_web_page,this);
		m_message_count = 0;
	}

	void ChatSessionImpl::loadTheme(const QString& path, const QString& variant)
	{
		m_chat_style_output->loadTheme(path,variant);
		m_chat_style_output->preparePage(m_web_page,this);
	}

	void ChatSessionImpl::setVariant(const QString& variant)
	{
		m_chat_style_output->setVariant(variant);
		m_chat_style_output->reloadStyle(m_web_page);
	}

	void ChatSessionImpl::loadHistory()
	{
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("general/history");
		int max_num = adium_chat.value<int>("maxDisplayMessages",5);
		MessageList messages = History::instance()->read(getUnit(),max_num);
		foreach (Message mess, messages)
			appendMessage(mess);
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
		Message tmp_message = message;
		//TODO add normal check if contact is null
		if (!tmp_message.chatUnit())
		{
			//TODO create fake chat unit for unknown messages
			tmp_message.setChatUnit(getUnit());
		}
		History::instance()->store(tmp_message);
		bool same_from = false;
		bool isHistory = tmp_message.isIncoming() && tmp_message.time().isValid();
		if (isHistory)
		{
			m_previous_sender="";
		}
		QString item;
		if(tmp_message.text().startsWith("/me "))
		{
			tmp_message.setText(tmp_message.text().mid(3));
			item = m_chat_style_output->makeAction(this,tmp_message,true);
			m_previous_sender = "";
		}
		else if (tmp_message.property("service").toBool())
		{
			item = m_chat_style_output->makeStatus(message.text(), QDateTime::currentDateTime());
			m_previous_sender = "";
		}
		else
		{
			same_from = (m_previous_sender == (tmp_message.isIncoming()?"nme":"me"));
			item = m_chat_style_output->makeMessage(this, tmp_message, true,
															same_from );
			m_previous_sender = (tmp_message.isIncoming()?"nme":"me");
		}

		QString result = m_web_page->mainFrame()->evaluateJavaScript(QString("getEditedHtml(\"%1\", \"%2\");")
																	 .arg(validateCpp(item))
																	 .arg(m_message_count)).toString();
		QString jsTask = QString("append%2Message(\"%1\");").arg(
				result.isEmpty() ? item :
				validateCpp(result.replace("\\","\\\\")), same_from?"Next":"");
		if (!isHistory && !tmp_message.property("disableNotify").toBool())
			Notifications::sendNotification(tmp_message);
		m_web_page->mainFrame()->evaluateJavaScript(jsTask);
		if (!isHistory)
			History::instance()->store(message);
		if (result.isEmpty()) //TODO I'm not sure that it works well //FIXME
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


	ChatUnit* ChatSessionImpl::getUnit(bool create) const
	{
		return m_account->getUnit(m_session_id,create);
	}

	QVariant ChatSessionImpl::evaluateJavaScript(const QString &scriptSource)
	{
		if(m_web_page.isNull())
			return QVariant();
		return m_web_page->mainFrame()->evaluateJavaScript(scriptSource);
	}

	void ChatSessionImpl::activate(bool active)
	{

	}

	bool ChatSessionImpl::isActive()
	{

	}

}
