/****************************************************************************
 *  chatsessionimpl.h
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

#ifndef CHATSESSIONIMPL_H
#define CHATSESSIONIMPL_H
#include <libqutim/messagesession.h>
#include <QPointer>

class QAbstractItemModel;
class QWebPage;
using namespace qutim_sdk_0_3;
namespace AdiumChat
{
	class ChatStyleOutput;
	class ChatSessionImpl : public ChatSession
	{
		Q_OBJECT
	public:
		ChatSessionImpl (ChatUnit *unit, ChatLayer* chat );
		virtual ~ChatSessionImpl();
		virtual void addContact ( Contact* c );
		virtual void appendMessage ( const Message& message );
		virtual void chatStateChanged ( Contact* c, ChatState state );
		virtual void removeContact ( Contact* c );
		QWebPage *getPage() const;
		Account *getAccount() const;
		QString getId() const;
		ChatUnit *getUnit() const;
		void loadTheme(const QString& path, const QString& variant);
		void setVariant(const QString& variant);
		QString getVariant() const;
		QAbstractItemModel *getItemsModel() const;
		virtual void setActive(bool active = true);
		virtual bool isActive();
	public slots:
		QVariant evaluateJavaScript(const QString &scriptSource);
	protected:
		QScopedPointer<ChatStyleOutput> m_chat_style_output;
		void loadHistory();
		void show();
	private:
		QPointer<QWebPage> m_web_page;
		QPointer<ChatUnit> m_chat_unit;
		//additional info and flags
		QString m_previous_sender; //me or nme (not me) //FIXME need refactoring in future
		int m_message_count;
		bool m_active;
		bool m_showed;
		bool m_store_service_messages;
	signals:
		void removed(Account *,const QString &);//may be removed in future
	};
}
#endif // CHATSESSIONIMPL_H
