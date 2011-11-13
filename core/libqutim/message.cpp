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

namespace qutim_sdk_0_3
{
/*	class QmlMessage : public QObject
 {
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY onTextChanged)
  Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY onDateTimeChanged)
  Q_PROPERTY(bool incoming READ isIncoming WRITE setIncoming NOTIFY onIncomingChanged)
  Q_PROPERTY(QObject* chatUnit READ chatUnit WRITE setChatUnit NOTIFY onChatUnitChanged)
  Q_PROPERTY(quint64 id READ id CONSTANT)
 public:
  QmlMessage(Message &message, QObject *parent = 0) : QObject(parent), m_message(&message), m_delete(false)
  {
   foreach(const QByteArray &name, message.dynamicPropertyNames())
 setProperty(name, message.property(name));
  }
  QmlMessage(QObject *parent = 0) : QObject(parent), m_message(new Message), m_delete(true) {}
  ~QmlMessage() { if(m_delete) delete m_message; }
  Message message() const { return *m_message; }

  QString text() { return m_message->text(); }
  QDateTime dateTime() { return m_message->time(); }
  bool isIncoming() { return m_message->isIncoming(); }
  QObject *chatUnit() { return m_message->chatUnit(); }
  quint64 id() { return m_message->id(); }

  void setText(const QString &t)
  {
   m_message->setText(t);
   emit onTextChanged(t);
  }
  void setDateTime(const QDateTime &dt)
  {
   m_message->setTime(dt);
   emit onDateTimeChanged(dt);
  }
  void setIncoming(bool i)
  {
   m_message->setIncoming(i);
   emit onIncomingChanged(i);
  }
  void setChatUnit(QObject *obj)
  {
   m_message->setChatUnit(qobject_cast<ChatUnit *>(obj));
   emit onChatUnitChanged(obj);
  }

 protected:
  bool event(QEvent *ev)
  {
   if (ev->type() == QEvent::DynamicPropertyChange) {
 QDynamicPropertyChangeEvent *event = static_cast<QDynamicPropertyChangeEvent *>(ev);
 m_message->setProperty(event->propertyName(), property(event->propertyName()));
   }
  }

 signals:
  void onTextChanged(const QString &);
  void onDateTimeChanged(const QDateTime &);
  void onIncomingChanged(bool);
  void onChatUnitChanged(QObject *);

 private:
  bool m_delete;
  Message *m_message;
 };
*/
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
		DynamicPropertyData(o), text(o.text), time(o.time),
		in(o.in), chatUnit(o.chatUnit), id(++message_id) {}
	~MessagePrivate() {}
	QString text;
	QDateTime time;
	bool in;
	QWeakPointer<ChatUnit> chatUnit;
	quint64 id;
	QVariant getText() const { return text; }
	void setText(const QVariant &val) { text = val.toString(); }
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
<< "time"
<< "in"
<< "chatUnit";
static QList<Getter> getters   = QList<Getter>()
<< static_cast<Getter>(&MessagePrivate::getText)
<< static_cast<Getter>(&MessagePrivate::getTime)
<< static_cast<Getter>(&MessagePrivate::getIn)
<< static_cast<Getter>(&MessagePrivate::getChatUnit);
static QList<Setter> setters   = QList<Setter>()
<< static_cast<Setter>(&MessagePrivate::setText)
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

Q_GLOBAL_STATIC(QWeakPointer<QTextDocument>, document)

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

