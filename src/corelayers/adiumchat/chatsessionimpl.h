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
#include <QTimer>

class QAbstractItemModel;
class QWebPage;
namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;
		class ChatStyleOutput;
		class ChatSessionModel;
		class ChatSessionImplPrivate;
		class ChatSessionImpl : public ChatSession
		{
			Q_OBJECT
			Q_DECLARE_PRIVATE(ChatSessionImpl)
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
			ChatState getChatState() const;
		signals:
			void buddiesChanged();
		public slots:
			QVariant evaluateJavaScript(const QString &scriptSource);
		private:
			QScopedPointer<ChatSessionImplPrivate> d_ptr;
		};
	}
}
#endif // CHATSESSIONIMPL_H
