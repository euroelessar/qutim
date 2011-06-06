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

#ifndef SCRIPTMESSAGE_H
#define SCRIPTMESSAGE_H

#include <QScriptClass>
#include <QScriptString>
#include <qutim/message.h>

namespace qutim_sdk_0_3
{
class ScriptMessage : public QScriptClass
{
public:
    ScriptMessage(QScriptEngine *engine);
	~ScriptMessage();
	
	virtual QueryFlags queryProperty(const QScriptValue &object,
									 const QScriptString &name,
									 QueryFlags flags, uint *id);
	virtual QScriptValue property(const QScriptValue &object,
								  const QScriptString &name, uint id);
	virtual void setProperty(QScriptValue &object, const QScriptString &name,
							 uint id, const QScriptValue &value);
	virtual QScriptValue::PropertyFlags propertyFlags(
			const QScriptValue &object, const QScriptString &name, uint id);
	virtual QScriptClassPropertyIterator *newIterator(const QScriptValue &object);
	virtual QScriptValue prototype() const;
	virtual QString name() const;
	
private:
	QScriptString m_incoming;
	QScriptValue m_prototype;
};
}

#endif // SCRIPTMESSAGE_H
