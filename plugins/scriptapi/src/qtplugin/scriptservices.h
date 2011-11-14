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

#ifndef SCRIPTSERVICES_H
#define SCRIPTSERVICES_H

#include <QScriptClass>
#include <QScriptClassPropertyIterator>
#include <qutim/servicemanager.h>

namespace qutim_sdk_0_3
{
typedef QHash<QScriptString, ServicePointer<QObject> > ScriptServiceHash;

class ScriptServicesPropertyIterator : public QScriptClassPropertyIterator
{
public:
    ScriptServicesPropertyIterator(const QScriptValue &object, const ScriptServiceHash &hash);
	
    virtual bool hasNext() const;
    virtual void next();

    virtual bool hasPrevious() const;
    virtual void previous();

    virtual void toFront();
    virtual void toBack();

    virtual QScriptString name() const;
    virtual QScriptValue::PropertyFlags flags() const;
private:
	QHashIterator<ScriptServiceHash::key_type, ScriptServiceHash::mapped_type> m_it;
};

class ScriptServices : public QScriptClass
{
public:
    ScriptServices(QScriptEngine *engine);
	
	virtual QueryFlags queryProperty(const QScriptValue &object,
									 const QScriptString &name,
									 QueryFlags flags, uint *id);
	
	virtual QScriptValue property(const QScriptValue &object, const QScriptString &name, uint id);
	
	virtual void setProperty(QScriptValue &object, const QScriptString &name,
							 uint id, const QScriptValue &value);
	
	virtual QScriptValue::PropertyFlags propertyFlags(const QScriptValue &object,
	                                                  const QScriptString &name, uint id);
	
	virtual QScriptClassPropertyIterator *newIterator(const QScriptValue &object);
	
	virtual QString name() const;
private:
	ScriptServiceHash m_services;
};
}

#endif // SCRIPTSERVICES_H

