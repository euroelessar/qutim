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

#include "scriptmessageclass.h"
#include "scriptmessagepropertyiterator.h"
#include <QScriptEngine>
#include <qutim/message.h>
#include <qutim/chatunit.h>

using namespace qutim_sdk_0_3;

inline Message *get_value(const QScriptValue &obj)
{
	return obj.data().toVariant().value<Message*>();
}

ScriptMessageClass::ScriptMessageClass(QScriptEngine *engine) : QScriptClass(engine)
{
}

QScriptClass::QueryFlags ScriptMessageClass::queryProperty(const QScriptValue &object,
														   const QScriptString &name,
														   QueryFlags flags, uint *id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(flags);
	Q_UNUSED(id);
	return HandlesReadAccess | HandlesWriteAccess;
}

QScriptValue ScriptMessageClass::property(const QScriptValue &object,
										  const QScriptString &name, uint id)
{
	Q_UNUSED(id);
	Message *msg = get_value(object);
	QScriptValue value = engine()->toScriptValue(msg->property(name.toString().toUtf8()));
	return value;
}

void ScriptMessageClass::setProperty(QScriptValue &object, const QScriptString &name,
									 uint id, const QScriptValue &value)
{
	Q_UNUSED(id);
	Message *msg = get_value(object);
	msg->setProperty(name.toString().toUtf8(), value.toVariant());
}

QScriptValue::PropertyFlags ScriptMessageClass::propertyFlags(
		const QScriptValue &object, const QScriptString &name, uint id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(id);
	return 0;
}

QScriptClassPropertyIterator *ScriptMessageClass::newIterator(const QScriptValue &object)
{
	Message *msg = get_value(object);
	return new ScriptMessagePropertyIterator(object, msg);
}

//QScriptValue ScriptMessageClass::prototype() const
//{
//	return QScriptValue();
//}

QString ScriptMessageClass::name() const
{
	return QLatin1String("Message");
}
