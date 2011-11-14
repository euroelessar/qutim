/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef HISTORY_H
#define HISTORY_H

#include "message.h"
#include <QDateTime>

namespace qutim_sdk_0_3
{
	class ChatUnit;
	
#ifdef EXPERIMENTAL_HISTORY_API	
	class HistoryRequestPrivate;
	class HistoryReplyPrivate;
	class HistoryRequest;
	
	class HistoryReply : public QObject
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(HistoryReply)
	public:
		HistoryReply();
		~HistoryReply();
		
		HistoryRequest request() const;
		MessageList messages() const;
		int totalCount() const;
		int firstIndex() const;
		int lastIndex() const;
	signals:
		void ready();
	protected:
		void setMessages(const MessageList &messages);
		void setCount(int count);
		void setBoundaries(int first, int last);
	private:
		QScopedPointer<HistoryReplyPrivate> d_ptr;
	};
	
	class HistoryRequest
	{
	public:
		HistoryRequest(ChatUnit *unit);
		HistoryRequest(const HistoryRequest &other);
		~HistoryRequest();
		HistoryRequest &operator =(const HistoryRequest &other);
//		enum Type { MessagesCount,  };
		
		HistoryReply *send();
	private:
		QSharedDataPointer<HistoryRequestPrivate> d_ptr;
	};

	class LIBQUTIM_EXPORT HistoryEngine : public QObject
	{
		Q_OBJECT
	public:
		HistoryEngine();
		virtual ~HistoryEngine();
		virtual HistoryReply *store(const Message &msg) = 0;
		virtual HistoryReply *request(const HistoryRequest &rule) = 0;
	};
#endif // EXPERIMENTAL_HISTORY_API	
	
	class LIBQUTIM_EXPORT History : public QObject
	{
		Q_OBJECT
	public:
		History();
		virtual ~History();
		static History *instance();
		virtual void store(const Message &message);
		virtual MessageList read(const ChatUnit *unit, const QDateTime &from, const QDateTime &to, int max_num);
		MessageList read(const ChatUnit *unit, const QDateTime &to, int max_num) { return read(unit, QDateTime(), to, max_num); }
		MessageList read(const ChatUnit *unit, int max_num) { return read(unit, QDateTime(), QDateTime::currentDateTime(), max_num); }
	public slots:
		virtual void showHistory(const ChatUnit *unit);
		virtual void virtual_hook(int id, void *data);
	};
}

#endif // HISTORY_H

