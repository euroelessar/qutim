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
