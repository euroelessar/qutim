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

#include "scriptmessagepropertyiterator.h"
#include <QScriptEngine>
#include <QScriptString>

using namespace qutim_sdk_0_3;

ScriptMessagePropertyIterator::ScriptMessagePropertyIterator(const QScriptValue &object, Message *msg) :
		QScriptClassPropertyIterator(object), m_msg(msg), m_id(-1)
{
}

bool ScriptMessagePropertyIterator::hasNext() const
{
	return m_id < 4 + m_msg->dynamicPropertyNames().size();
}

void ScriptMessagePropertyIterator::next()
{
	m_id++;
}

bool ScriptMessagePropertyIterator::hasPrevious() const
{
	return m_id > -1;
}

void ScriptMessagePropertyIterator::previous()
{
	m_id--;
}

void ScriptMessagePropertyIterator::toFront()
{
	m_id = -1;
}

void ScriptMessagePropertyIterator::toBack()
{
	m_id = 4 + m_msg->dynamicPropertyNames().size();
}

QScriptString ScriptMessagePropertyIterator::name() const
{
	switch (m_id) {
	case 0:
		return object().engine()->toStringHandle(QLatin1String("text"));
	case 1:
		return object().engine()->toStringHandle(QLatin1String("time"));
	case 2:
		return object().engine()->toStringHandle(QLatin1String("in"));
	case 3:
		return object().engine()->toStringHandle(QLatin1String("chatUnit"));
	default: {
		QByteArray latinName = m_msg->dynamicPropertyNames().value(m_id - 4);
		return object().engine()->toStringHandle(QLatin1String(latinName));
	}
	}
}

