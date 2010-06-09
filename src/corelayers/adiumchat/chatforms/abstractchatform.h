/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef ABSTRACTCHATFORM_H
#define ABSTRACTCHATFORM_H

#include <QWidget>
#include <QHash>
#include <QPointer>

namespace qutim_sdk_0_3 {
	class ChatSession;
}

namespace Core
{
	namespace AdiumChat
	{
		class AbstractChatWidget;
		class ChatSessionImpl;
		
		class AbstractChatForm : public QObject
		{
			Q_OBJECT
			Q_CLASSINFO("Service", "ChatForm")
			Q_CLASSINFO("Uses", "ChatLayer")
			Q_CLASSINFO("Uses", "IconLoader")
		public:
			explicit AbstractChatForm();
			~AbstractChatForm();
			Q_INVOKABLE QObject *textEdit(qutim_sdk_0_3::ChatSession *session);
			Q_INVOKABLE QWidgetList chatWidgets();
		protected:
			virtual AbstractChatWidget *createWidget(const QString &key, bool removeSessionOnClose) = 0;
		private slots:
			void onSessionCreated(qutim_sdk_0_3::ChatSession*);
			void onChatWidgetDestroyed(QObject *object);
			void onSessionActivated(bool active);
		private:
			QHash<QString, QPointer<AbstractChatWidget> > m_chatwidgets;
			QString getWidgetId(ChatSessionImpl *sess) const;
			AbstractChatWidget *findWidget(qutim_sdk_0_3::ChatSession *sess) const;
		};
	}
}

#endif // ABSTRACTCHATFORM_H
