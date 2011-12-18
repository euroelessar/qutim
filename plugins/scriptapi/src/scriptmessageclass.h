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

#ifndef SCRIPTMESSAGECLASS_H
#define SCRIPTMESSAGECLASS_H

#include <QScriptClass>
#include <qutim/message.h>

class ScriptMessageClass : public QScriptClass
{
public:
	ScriptMessageClass(QScriptEngine *engine);
	
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
	
//	virtual QScriptValue prototype() const;
	
	virtual QString name() const;
//	
//	virtual bool supportsExtension(Extension extension) const;
//	virtual QVariant extension(Extension extension,
//							   const QVariant &argument = QVariant());
};

#endif // SCRIPTMESSAGECLASS_H

