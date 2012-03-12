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

#include "scriptmessage.h"
#include "scriptenginedata.h"
#include "scriptmessagepropertyiterator.h"
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <qutim/debug.h>

namespace qutim_sdk_0_3
{
struct ScriptMessageData
{
	ScriptMessageData() : message(0) {}
	ScriptMessageData(const Message &msg) : ptr(QSharedPointer<Message>(new Message(msg))), message(ptr.data()) {}
	ScriptMessageData(Message *msg) : message(msg) {}
	ScriptMessageData(const ScriptMessageData &o) : ptr(o.ptr), message(o.message) {}
	ScriptMessageData &operator =(const ScriptMessageData &o)
	{
		ptr = o.ptr;
		message = o.message;
		return *this;
	}

	QSharedPointer<Message> ptr;
	Message *message;
};

inline Message *message_get_value(const QScriptValue &obj)
{
	ScriptMessageData data = obj.data().toVariant().value<ScriptMessageData>();
	return data.message;
}

QScriptValue messageToScriptValue(QScriptEngine *engine, const Message &message)
{
	ScriptMessageData messageData(message);
	QScriptValue data = engine->newVariant(qVariantFromValue(messageData));
	QScriptValue obj = engine->newObject(ScriptEngineData::data(engine)->message, data);
	return obj;
}

void messageFromScriptValue(const QScriptValue &obj, Message &message)
{
	if (obj.isObject()) {
		if (Message *msg = message_get_value(obj)) {
			message = *msg;
			return;
		}
		message = Message();
		QScriptValueIterator it(obj);
		while (it.hasNext()) {
			it.next();
			if (it.name() == QLatin1String("incoming"))
				message.setIncoming(it.value().toBool());
			else
				message.setProperty(it.name().toUtf8(), it.value().toVariant());
		}
	} else {
		message = Message(obj.toString());
	}
}

QScriptValue messagePtrToScriptValue(QScriptEngine *engine, Message * const &message)
{
	ScriptMessageData messageData(message);
	QScriptValue data = engine->newVariant(qVariantFromValue(messageData));
	QScriptValue obj = engine->newObject(ScriptEngineData::data(engine)->message, data);
	return obj;
}

void messagePtrFromScriptValue(const QScriptValue &obj, Message * &message)
{
	message = message_get_value(obj);
}

QScriptValue createMessage(QScriptContext *context, QScriptEngine *engine)
{
	Message message;
	if (context->argumentCount() >= 1) {
		QScriptValue arg = context->argument(0);
		messageFromScriptValue(arg, message);
	}
	return engine->toScriptValue(message);
}

ScriptMessage::ScriptMessage(QScriptEngine *engine) : QScriptClass(engine)
{
	debug() << Q_FUNC_INFO;
	m_incoming = engine->toStringHandle(QLatin1String("incoming"));
	ScriptEngineData::data(engine)->message = this;
	qScriptRegisterMetaType(engine, messageToScriptValue, messageFromScriptValue);
	qScriptRegisterMetaType(engine, messagePtrToScriptValue, messagePtrFromScriptValue);
	qRegisterMetaType<qutim_sdk_0_3::Message>("qutim_sdk_0_3::Message&");
	m_prototype = engine->newObject(this);
	QScriptValue ctor = engine->newFunction(createMessage);
    engine->globalObject().setProperty(name(), ctor);
}

ScriptMessage::~ScriptMessage()
{
	debug() << Q_FUNC_INFO;
}

QScriptClass::QueryFlags ScriptMessage::queryProperty(const QScriptValue &object,
                                                      const QScriptString &name,
                                                      QueryFlags flags, uint *id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(flags);
	Q_UNUSED(id);
	return HandlesReadAccess | HandlesWriteAccess;
}

QScriptValue ScriptMessage::property(const QScriptValue &object, const QScriptString &name, uint id)
{
	Q_UNUSED(id);
	Message *msg = message_get_value(object);
	if (name == m_incoming)
		return msg->isIncoming();
	return engine()->toScriptValue(msg->property(name.toString().toUtf8()));
}
/*
var conference = qutim.protocol("jabber").account("euroelessar@jabber.ru").unit("talks@conference.qutim.org", false);
var msg = new Message;
msg.text = "Hi!";
conference.sendMessage(msg);
*/
void ScriptMessage::setProperty(QScriptValue &object, const QScriptString &name,
                                uint id, const QScriptValue &value)
{
	Q_UNUSED(id);
	Message *msg = message_get_value(object);
	if (name == m_incoming)
		msg->setIncoming(value.toBool());
	else
		msg->setProperty(name.toString().toUtf8(), value.toVariant());
}

QScriptValue::PropertyFlags ScriptMessage::propertyFlags(const QScriptValue &object,
                                                         const QScriptString &name, uint id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(id);
	return 0;
}

QScriptClassPropertyIterator *ScriptMessage::newIterator(const QScriptValue &object)
{
	Message *msg = message_get_value(object);
	return new ScriptMessagePropertyIterator(object, msg);
}

QScriptValue ScriptMessage::prototype() const
{
	return m_prototype;
}

QString ScriptMessage::name() const
{
	return QLatin1String("Message");
}
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ScriptMessageData)

