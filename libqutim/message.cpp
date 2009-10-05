#include "message.h"
#include <QDateTime>

namespace qutim_sdk_0_3
{
	class MessagePrivate : public QSharedData
	{
	public:
		MessagePrivate() : in(false) {}
		MessagePrivate(const MessagePrivate &o)
				: QSharedData(o), message(o.message), time(o.time), names(o.names), values(o.values) {}
		~MessagePrivate() {}
		QString message;
		QDateTime time;
		bool in;
		QVariant getMessage() const { return message; }
		void setMessage(const QVariant &val) { message = val.toString(); }
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
										 << "message"
										 << "time"
										 << "in";
		static QList<Getter> getters   = QList<Getter>()
										 << &MessagePrivate::getMessage
										 << &MessagePrivate::getTime
										 << &MessagePrivate::getIn;
		static QList<Setter> setters   = QList<Setter>()
										 << &MessagePrivate::setMessage
										 << &MessagePrivate::setTime
										 << &MessagePrivate::setIn;
	}

	Message::Message() : p(new MessagePrivate)
	{
	}

	Message::Message(const QString &message) : p(new MessagePrivate)
	{
		p->message = message;
	}

	Message::Message(const Message &other) : p(other.p)
	{
	}

	Message::~Message()
	{
	}

	QVariant Message::property(const char *name) const
	{
		QByteArray prop = name;
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
		QByteArray prop = name;
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

	const QString &Message::message() const
	{
		return p->message;
	}

	void Message::setMessage(const QString &message)
	{
		p->message = message;
	}

	const QDateTime &Message::time() const
	{
		return p->time;
	}

	void Message::setTime(const QDateTime &time)
	{
		p->time = time;
	}

	bool Message::in() const
	{
		return p->in;
	}

	void Message::setIn(bool input)
	{
		p->in = input;
	}
}
