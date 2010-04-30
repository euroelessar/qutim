/****************************************************************************
 *  chatsessionimpl.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <QTextDocument>
#include <QPointer>

class QAbstractItemModel;
class QWebPage;
using namespace qutim_sdk_0_3;
namespace AdiumChat
{
	class ChatStyleOutput;
	class ChatSessionModel;
	class ChatSessionImpl : public ChatSession
	{
		Q_OBJECT
	public:
		ChatSessionImpl (ChatUnit *unit, ChatLayer* chat );
		virtual ~ChatSessionImpl();
		virtual void addContact ( Buddy* c );
		virtual qint64 appendMessage(Message &message);
		virtual void removeContact ( Buddy* c );
		QWebPage *getPage() const;
		Account *getAccount() const;
		QString getId() const;
		ChatUnit *getUnit() const;
		virtual QTextDocument *getInputField();
		virtual void markRead(quint64 id = Q_UINT64_C(0xffffffffffffffff));
		virtual MessageList unread() const;
		void loadTheme(const QString& path, const QString& variant);
		void setVariant(const QString& variant);
		void setCustomCSS(const QString &css);
		QString getVariant() const;
		QAbstractItemModel *getModel() const;
		virtual void setActive(bool active = true);
		virtual void setChatUnit(ChatUnit* unit);
		virtual bool isActive();
		QMenu *menu();
		bool event(QEvent *);
		void setChatState(ChatState state);
		ChatState getChatState() const {return m_myself_chat_state;};
	signals:
		void buddiesChanged();
	public slots:
		QVariant evaluateJavaScript(const QString &scriptSource);
	protected:
		QScopedPointer<ChatStyleOutput> m_chat_style_output;
		void loadHistory();
		virtual void timerEvent(QTimerEvent *event);
	protected slots:
		void onStatusChanged(qutim_sdk_0_3::Status);
		void onLinkClicked(const QUrl &url);
	private:
		void statusChanged(Contact *contact, bool silent = false);
		QPointer<QWebPage> m_web_page;
		QPointer<ChatUnit> m_chat_unit;
		QPointer<QTextDocument> m_input;
		QPointer<QMenu> m_menu;
		ChatSessionModel *m_model;
		//additional info and flags
		QString m_previous_sender; // null if outcoming
		bool m_skipOneMerge;
		bool m_active;
		bool m_store_service_messages;
		int m_inactive_timer;
		MessageList m_unread;
		ChatState m_myself_chat_state;
	};
}
#endif // CHATSESSIONIMPL_H
