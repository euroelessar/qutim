/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "message.h"
#include "dynamicpropertydata_p.h"
#include <QDateTime>
#include <QScriptEngine>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QTextDocument>
#include "chatunit.h"
#include "account.h"
#include "protocol.h"
#include <QDebug>

QDebug operator<<(QDebug dbg, const qutim_sdk_0_3::Message &msg)
{
	dbg.nospace() << QLatin1String("Ureen::Message(")
				  << QLatin1String("id: ") << msg.id()
				  << QLatin1String("chatUnit: ") << msg.chatUnit()
				  << QLatin1String("isIncoming: ") << msg.isIncoming()
				  << QLatin1String("text: ") << msg.text()
				  << QLatin1String("time: ") << msg.time()
				  << QLatin1String("properties: (");
	foreach (QByteArray name, msg.dynamicPropertyNames())
		dbg.nospace() << name << ": " << msg.property(name);
	return dbg.nospace() << QLatin1String(") )");
}

namespace qutim_sdk_0_3
{
QScriptValue messageToScriptValue(QScriptEngine *engine, const Message &mes)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("time", engine->newDate(mes.time()));
	obj.setProperty("chatUnit", engine->newQObject(const_cast<ChatUnit *>(mes.chatUnit())));
	obj.setProperty("text", mes.text());
	obj.setProperty("in", mes.isIncoming());
	foreach(const QByteArray &name, mes.dynamicPropertyNames())
		obj.setProperty(QString::fromUtf8(name), engine->newVariant(mes.property(name)));
	return obj;
}

void messageFromScriptValue(const QScriptValue &obj, Message &mes)
{
	QScriptValueIterator it(obj);
	while (it.hasNext()) {
		it.next();
		mes.setProperty(it.name().toUtf8(), it.value().toVariant());
	}
}

void Message::scriptRegister(QScriptEngine *engine)
{
	qScriptRegisterMetaType(engine, &messageToScriptValue, &messageFromScriptValue);
}

static quint64 message_id = 0;

class MessagePrivate : public DynamicPropertyData
{
public:
	MessagePrivate() :
		time(QDateTime::currentDateTime()), in(false),
		id(++message_id) {}
	MessagePrivate(const MessagePrivate &o) :
		DynamicPropertyData(o), text(o.text), html(o.html), time(o.time),
		in(o.in), chatUnit(o.chatUnit), id(++message_id) {}
	~MessagePrivate() {}
	QString text;
	QString html;
	QDateTime time;
	bool in;
	QPointer<ChatUnit> chatUnit;
	quint64 id;
	QVariant getText() const { return text; }
	void setText(const QVariant &val) { text = val.toString(); }
	QVariant getHtml() const {
		if (html.isEmpty()) {
			QString &mutableHtml = const_cast<QString&>(html);
			mutableHtml = Qt::escape(text);
			// keep leading whitespaces
			mutableHtml.replace(QLatin1String("\n "), QLatin1String("<br/>&nbsp;"));
			mutableHtml.replace(QLatin1Char('\n'), QLatin1String("<br/>"));
			// replace tabs by 4 spaces
			mutableHtml.replace(QLatin1Char('\t'), QLatin1String("&nbsp; &nbsp; "));
			// keep multiple whitespaces
			mutableHtml.replace(QLatin1String("  "), QLatin1String(" &nbsp;"));
		}
		return html;
	}
	void setHtml(const QVariant &val) { html = val.toString(); }
	QVariant getTime() const { return time; }
	void setTime(const QVariant &val) { time = val.toDateTime(); }
	QVariant getIn() const { return in;}
	void setIn(const QVariant &input) { in = input.toBool(); }
	void setChatUnit (const QVariant &val) { chatUnit = val.value<ChatUnit *>(); }
	QVariant getChatUnit() const { return QVariant::fromValue(chatUnit.data()); }
};

namespace CompiledProperty
{
static QList<QByteArray> names = QList<QByteArray>()
								 << "text"
								 << "html"
								 << "time"
								 << "in"
								 << "chatUnit";
static QList<Getter> getters   = QList<Getter>()
								 << static_cast<Getter>(&MessagePrivate::getText)
								 << static_cast<Getter>(&MessagePrivate::getHtml)
								 << static_cast<Getter>(&MessagePrivate::getTime)
								 << static_cast<Getter>(&MessagePrivate::getIn)
								 << static_cast<Getter>(&MessagePrivate::getChatUnit);
static QList<Setter> setters   = QList<Setter>()
								 << static_cast<Setter>(&MessagePrivate::setText)
								 << static_cast<Setter>(&MessagePrivate::setHtml)
								 << static_cast<Setter>(&MessagePrivate::setTime)
								 << static_cast<Setter>(&MessagePrivate::setIn)
								 << static_cast<Setter>(&MessagePrivate::setChatUnit);
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

Message &Message::operator =(const Message &other)
{
	p = other.p;
	return *this;
}

QVariant Message::property(const char *name, const QVariant &def) const
{
	return p->property(name, def, CompiledProperty::names, CompiledProperty::getters);
}

void Message::setProperty(const char *name, const QVariant &value)
{
	return p->setProperty(name, value, CompiledProperty::names, CompiledProperty::setters);
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

QString Message::html() const
{
	return p->getHtml().toString();
}

void Message::setHtml(const QString &html)
{
	p->html = html;
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

ChatUnit* Message::chatUnit() const
{
	return p->chatUnit.data();
}

quint64 Message::id() const
{
	return p->id;
}

void Message::setChatUnit ( ChatUnit* chatUnit )
{
	p->chatUnit = chatUnit;
}

MessageReceiptEvent::MessageReceiptEvent(quint64 id, bool success) :
	QEvent(eventType()), i(id), s(success)
{
}

QEvent::Type MessageReceiptEvent::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 101));
	return type;
}

Q_GLOBAL_STATIC(QPointer<QTextDocument>, document)

QString unescape(const QString &html)
{
	if (document()->isNull())
		(*document()) = new QTextDocument;
	document()->data()->setHtml(html);
	QString plainText = document()->data()->toPlainText();
	document()->data()->clearUndoRedoStacks();
	return plainText;
}
}

