/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "scriptservices.h"
#include <QScriptEngine>

namespace qutim_sdk_0_3
{
ScriptServicesPropertyIterator::ScriptServicesPropertyIterator(const QScriptValue &object, const ScriptServiceHash &hash)
    : QScriptClassPropertyIterator(object), m_it(hash)
{
}

bool ScriptServicesPropertyIterator::hasNext() const
{
	return m_it.hasNext();
}

void ScriptServicesPropertyIterator::next()
{
	m_it.next();
}

bool ScriptServicesPropertyIterator::hasPrevious() const
{
	return m_it.hasPrevious();
}

void ScriptServicesPropertyIterator::previous()
{
	m_it.previous();
}

void ScriptServicesPropertyIterator::toFront()
{
	m_it.toFront();
}

void ScriptServicesPropertyIterator::toBack()
{
	m_it.toBack();
}

QScriptString ScriptServicesPropertyIterator::name() const
{
	return m_it.key();
}

QScriptValue::PropertyFlags ScriptServicesPropertyIterator::flags() const
{
	return QScriptValue::ReadOnly | QScriptValue::Undeletable;
}

ScriptServices::ScriptServices(QScriptEngine *engine) : QScriptClass(engine)
{
	const QList<QByteArray> names = ServiceManager::names();
	for (int i = 0; i < names.size(); ++i) {
		QScriptString name = engine->toStringHandle(QString::fromUtf8(names[i]));
		m_services.insert(name, ServicePointer<QObject>(names[i]));
	}
}

QScriptClass::QueryFlags ScriptServices::queryProperty(const QScriptValue &object,
                                                      const QScriptString &name,
                                                      QueryFlags flags, uint *id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(flags);
	Q_UNUSED(id);
	return HandlesReadAccess;
}

QScriptValue ScriptServices::property(const QScriptValue &object, const QScriptString &name, uint id)
{
	Q_UNUSED(id);
	Q_UNUSED(object);
	return engine()->newQObject(m_services.value(name));
}

void ScriptServices::setProperty(QScriptValue &object, const QScriptString &name,
                                uint id, const QScriptValue &value)
{
	Q_UNUSED(id);
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(value);
}

QScriptValue::PropertyFlags ScriptServices::propertyFlags(const QScriptValue &object,
                                                         const QScriptString &name, uint id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(id);
	return 0;
}

QScriptClassPropertyIterator *ScriptServices::newIterator(const QScriptValue &object)
{
	return new ScriptServicesPropertyIterator(object, m_services);
}

QString ScriptServices::name() const
{
	return QLatin1String("ServiceManager");
}
}
