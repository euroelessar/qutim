/****************************************************************************
 *  message.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "message.h"
#include <QDateTime>

namespace qutim_sdk_0_3
{
	class MessagePrivate : public QSharedData
	{
	public:
		MessagePrivate() : in(false) {}
		MessagePrivate(const MessagePrivate &o)
				: QSharedData(o), text(o.text), time(o.time), names(o.names), values(o.values) {}
		~MessagePrivate() {}
		QString text;
		QDateTime time;
		bool in;
		QVariant getText() const { return text; }
		void setText(const QVariant &val) { text = val.toString(); }
		QVariant getTime() const { return time; }
		void setTime(const QVariant &val) { time = val.toDateTime(); }
		QVariant getIn() const { return in;}
		void setIn(const QVariant &input) { in = input.toBool(); }
		QList<QByteArray> names;
		QList<QVariant> values;
	};

	namespace CompiledProperty
	{
		typedef QVariant (MessagePrivate::*Getter)() const;
		typedef void (MessagePrivate::*Setter)(const QVariant &variant);
		static QList<QByteArray> names = QList<QByteArray>()
										 << "text"
										 << "time"
										 << "in";
		static QList<Getter> getters   = QList<Getter>()
										 << &MessagePrivate::getText
										 << &MessagePrivate::getTime
										 << &MessagePrivate::getIn;
		static QList<Setter> setters   = QList<Setter>()
										 << &MessagePrivate::setText
										 << &MessagePrivate::setTime
										 << &MessagePrivate::setIn;
	}

	Message::Message() : p(new MessagePrivate)
	{
	}

	Message::Message(const QString &text) : p(new MessagePrivate)
	{
		p->text = text;
	}

	Message::Message(const Message &other) : p(other.p)
	{
	}

	Message::~Message()
	{
	}

	QVariant Message::property(const char *name) const
	{
		QByteArray prop = QByteArray::fromRawData(name, strlen(name));
		int id = CompiledProperty::names.indexOf(prop);
		if(id < 0)
		{
			id = p->names.indexOf(prop);
			if(id < 0)
				return QVariant();
			return p->values.at(id);
		}
		return (p->*CompiledProperty::getters.at(id))();
	}

	void Message::setProperty(const char *name, const QVariant &value)
	{
		QByteArray prop = QByteArray::fromRawData(name, strlen(name));
		int id = CompiledProperty::names.indexOf(prop);
		if(id < 0)
		{
			id = p->names.indexOf(prop);
			if(!value.isValid())
			{
				if(id < 0)
					return;
				p->names.removeAt(id);
				p->values.removeAt(id);
			}
			else
			{
				if(id < 0)
				{
					prop.detach();
					p->names.append(prop);
					p->values.append(value);
				}
				else
					p->values[id] = value;
			}
		}
		else
			(p->*CompiledProperty::setters.at(id))(value);
	}

	QList<QByteArray> Message::dynamicPropertyNames() const
	{
		return p->names;
	}

	const QString &Message::text() const
	{
		return p->text;
	}

	void Message::setText(const QString &text)
	{
		p->text = text;
	}

	const QDateTime &Message::time() const
	{
		return p->time;
	}

	void Message::setTime(const QDateTime &time)
	{
		p->time = time;
	}

	bool Message::isIncoming() const
	{
		return p->in;
	}

	void Message::setIncoming(bool input)
	{
		p->in = input;
	}
}
