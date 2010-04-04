/****************************************************************************
 *  message.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef LIBQUTIM_MESSAGE_H
#define LIBQUTIM_MESSAGE_H

#include "libqutim_global.h"
#include <QSharedData>
#include <QVariant>
#include <QEvent>

class QScriptEngine;

namespace qutim_sdk_0_3
{
	class ChatUnit;
	class MessagePrivate;

	class LIBQUTIM_EXPORT Message
	{
	public:
		Message();
		Message(const QString &text);
		Message(const Message &other);
		virtual ~Message();
		Message &operator =(const Message &other);
		static void scriptRegister(QScriptEngine *engine);
		const QString &text() const;
		void setText(const QString &text);
		const QDateTime &time() const;
		void setTime(const QDateTime &time);
		void setIncoming(bool input);
		bool isIncoming() const;
		void setChatUnit (ChatUnit *chatUnit);
		const ChatUnit *chatUnit() const;
		quint64 id() const;
		// TODO: merge methods
		QVariant property(const char *name) const;
		QVariant property(const char *name, const QVariant &def) const;
		template<typename T>
		T property(const char *name, const T &def) const
		{ return qVariantValue<T>(property(name, qVariantFromValue<T>(def))); }
		void setProperty(const char *name, const QVariant &value);
		QList<QByteArray> dynamicPropertyNames() const;
	private:
		QSharedDataPointer<MessagePrivate> p;
	};

	class LIBQUTIM_EXPORT MessageReceiptEvent : public QEvent
	{
	public:
		MessageReceiptEvent(quint64 id, bool success);
		inline quint64 id() const { return i; }
		inline bool success() const { return s; }
		static QEvent::Type eventType();
	protected:
		quint64 i;
		bool s;
	};

	LIBQUTIM_EXPORT QString unescape(const QString &html);

	typedef QList<Message> MessageList;
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Message)
Q_DECLARE_METATYPE(qutim_sdk_0_3::MessageList)

#endif // LIBQUTIM_MESSAGE_H
