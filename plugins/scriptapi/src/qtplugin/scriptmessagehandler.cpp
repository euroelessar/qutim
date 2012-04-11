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

#include "scriptmessagehandler.h"
#include "scriptenginedata.h"
#include <QScriptEngine>

namespace qutim_sdk_0_3
{
class ScriptMessageHandlerObject : public MessageHandler
{
public:
	typedef QSharedPointer<ScriptMessageHandlerObject> Ptr;

	ScriptMessageHandlerObject() {}
	
	virtual Result doHandle(Message &message, QString *)
	{
		if (m_handler.isFunction()) {
			QScriptValueList args;
			args << m_that.engine()->toScriptValue(qVariantFromValue(&message));
			QScriptValue ret = m_handler.call(m_that, args);
			if (ret.isNumber())
				return static_cast<Result>(ret.toInt32());
		}
		return Accept;
	}
	
	void setThat(const QScriptValue &that) { m_that = that; }
	QScriptValue handler() { return m_handler; }
	void setHandler(const QScriptValue &handler) { m_handler = handler; }
private:
	QScriptValue m_that;
	QScriptValue m_handler;
};

ScriptMessageHandlerObject::Ptr get_value(const QScriptValue &obj)
{
	return obj.data().toVariant().value<ScriptMessageHandlerObject::Ptr>();
}

QScriptValue createMessageHandler(QScriptContext *context, QScriptEngine *engine)
{
	ScriptMessageHandler *messageHandler = ScriptEngineData::data(engine)->messageHandler;
	ScriptMessageHandlerObject::Ptr data = ScriptMessageHandlerObject::Ptr::create();
	QScriptValue value = engine->toScriptValue(qVariantFromValue(data));
	QScriptValue object = engine->newObject(messageHandler, value);
	data->setThat(object);
	data->setHandler(context->argument(0));
	return object;
}

QScriptValue messageHandlerRegister(QScriptContext *context, QScriptEngine *engine)
{
	int priority = MessageHandler::NormalPriortity;
	if (context->argument(0).isNumber())
		priority = context->argument(0).toInt32();
	if (MessageHandler *handler = get_value(context->thisObject()).data())
		MessageHandler::registerHandler(handler, QLatin1String("SomeScript"), priority, priority);
	return engine->undefinedValue();
}

QScriptValue messageHandlerUnregister(QScriptContext *context, QScriptEngine *engine)
{
	if (MessageHandler *handler = get_value(context->thisObject()).data())
		MessageHandler::unregisterHandler(handler);
	return engine->undefinedValue();
}

ScriptMessageHandler::ScriptMessageHandler(QScriptEngine *engine) : QScriptClass(engine)
{
	m_handler = engine->toStringHandle(QLatin1String("handler"));
	m_prototype = engine->newObject(this);
	m_prototype.setProperty(QLatin1String("register"), engine->newFunction(messageHandlerRegister));
	m_prototype.setProperty(QLatin1String("unregister"), engine->newFunction(messageHandlerUnregister));
	m_prototype.setProperty(QLatin1String("Accept"), MessageHandler::Accept, QScriptValue::ReadOnly);
	m_prototype.setProperty(QLatin1String("Reject"), MessageHandler::Reject, QScriptValue::ReadOnly);
	m_prototype.setProperty(QLatin1String("Error"),  MessageHandler::Error,  QScriptValue::ReadOnly);
	m_prototype.setProperty(QLatin1String("LowPriority"), MessageHandler::LowPriority,
	                        QScriptValue::ReadOnly);
	m_prototype.setProperty(QLatin1String("ChatInPriority"), MessageHandler::ChatInPriority,
	                        QScriptValue::ReadOnly);
	m_prototype.setProperty(QLatin1String("NormalPriortity"), MessageHandler::NormalPriortity,
	                        QScriptValue::ReadOnly);
	m_prototype.setProperty(QLatin1String("ChatOutPriority"), MessageHandler::ChatOutPriority,
	                        QScriptValue::ReadOnly);
	m_prototype.setProperty(QLatin1String("HighPriority"), MessageHandler::HighPriority,
	                        QScriptValue::ReadOnly);
	QScriptValue ctor = engine->newFunction(createMessageHandler);
	engine->globalObject().setProperty(name(), ctor);
}

QScriptClass::QueryFlags ScriptMessageHandler::queryProperty(const QScriptValue &object,
                                                             const QScriptString &name,
                                                             QueryFlags flags, uint *id)
{
	Q_UNUSED(object);
	Q_UNUSED(flags);
	Q_UNUSED(id);
	if (name == m_handler)
		return HandlesReadAccess | HandlesWriteAccess;
	return 0;
}

QScriptValue ScriptMessageHandler::property(const QScriptValue &object, const QScriptString &name, uint id)
{
	Q_UNUSED(id);
	if (name == m_handler)
		return get_value(object)->handler();
	return engine()->undefinedValue();
}

void ScriptMessageHandler::setProperty(QScriptValue &object, const QScriptString &name,
                                uint id, const QScriptValue &value)
{
	Q_UNUSED(id);
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(value);
}

QScriptValue::PropertyFlags ScriptMessageHandler::propertyFlags(const QScriptValue &object,
                                                         const QScriptString &name, uint id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(id);
	return 0;
}

QScriptClassPropertyIterator *ScriptMessageHandler::newIterator(const QScriptValue &object)
{
	Q_UNUSED(object);
	return 0;
}

QScriptValue ScriptMessageHandler::prototype() const
{
	return m_prototype;
}

QString ScriptMessageHandler::name() const
{
	return QLatin1String("MessageHandler");
}
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ScriptMessageHandlerObject::Ptr)

