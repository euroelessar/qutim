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

#include "scriptdataitem.h"
#include "scriptenginedata.h"
#include <qutim/dataforms.h>
#include <qutim/debug.h>
#include <QDebug>
#include <QScriptEngine>
#include <QScriptValueIterator>

namespace qutim_sdk_0_3
{
struct ScriptDataItemPtr
{
	QSharedPointer<DataItem> item;
	
	static ScriptDataItemPtr create(const DataItem &item)
	{
		ScriptDataItemPtr ptr = { QSharedPointer<DataItem>(new DataItem(item)) };
		return ptr;
	}
	
	static QVariant variant(const DataItem &item)
	{
		return qVariantFromValue(create(item));
	}
};

DataItem *get_data_item(const QScriptValue &obj)
{
	ScriptDataItemPtr data = obj.data().toVariant().value<ScriptDataItemPtr>();
	return data.item.data();
}

QScriptValue dataItemToScriptValue(QScriptEngine *engine, const DataItem &item)
{
	QScriptValue data = engine->newVariant(ScriptDataItemPtr::variant(item));
	QScriptValue obj = engine->newObject(ScriptEngineData::data(engine)->dataItem, data);
	return obj;
}

void dataItemFromScriptValue(const QScriptValue &obj, DataItem &dataItem)
{
	if (obj.isObject()) {
		if (DataItem *item = get_data_item(obj)) {
			dataItem = *item;
			return;
		}
		dataItem = DataItem();
		QScriptValueIterator it(obj);
		while (it.hasNext()) {
			it.next();
			dataItem.setProperty(it.name().toUtf8(), it.value().toVariant());
		}
	} else {
		dataItem = DataItem(obj.toVariant().value<LocalizedString>());
	}
}

QScriptValue createDateItem(QScriptContext *context, QScriptEngine *engine)
{
	DataItem item;
	if (context->argumentCount() >= 1)
		dataItemFromScriptValue(context->argument(0), item);
	return engine->toScriptValue(item);
}

QScriptValue scriptSubitem(QScriptContext *context, QScriptEngine *engine)
{
	DataItem *item = get_data_item(context->thisObject());
	bool recursive = context->argumentCount() > 1 && context->argument(1).toBool();
	DataItem subitem = item->subitem(context->argument(0).toString(), recursive);
	return engine->toScriptValue(subitem);
}

QScriptValue scriptSubitems(QScriptContext *context, QScriptEngine *engine)
{
	DataItem *item = get_data_item(context->thisObject());
	return engine->toScriptValue(item->subitems());
}

ScriptDataItemPropertyIterator::ScriptDataItemPropertyIterator(const QScriptValue &object)
    : QScriptClassPropertyIterator(object), m_id(-1)
{
	DataItem *item = get_data_item(object);
	QScriptEngine *engine = object.engine();
	const char *names[] = {
		"name",
	    "title",
	    "data",
	    "maxCount",
	    "defaultSubitem",
	    "onDataChangedReceiver",
	    "onDataChangedMethod"
	};
	for (uint i = 0; i < sizeof(names)/sizeof(names[0]); ++i)
		m_names << engine->toStringHandle(QLatin1String(names[i]));
	const QList<QByteArray> dynamicNames = item->dynamicPropertyNames();
	for (int i = 0; i < dynamicNames.size(); ++i)
		m_names << engine->toStringHandle(QLatin1String(dynamicNames[i]));
}

bool ScriptDataItemPropertyIterator::hasNext() const
{
	return m_id < m_names.size();
}

void ScriptDataItemPropertyIterator::next()
{
	++m_id;
}

bool ScriptDataItemPropertyIterator::hasPrevious() const
{
	return m_id > 0;
}

void ScriptDataItemPropertyIterator::previous()
{
	--m_id;
}

void ScriptDataItemPropertyIterator::toFront()
{
	m_id = -1;
}

void ScriptDataItemPropertyIterator::toBack()
{
	m_id = m_names.size();
}

QScriptString ScriptDataItemPropertyIterator::name() const
{
	return m_names[m_id];
}

ScriptDataItem::ScriptDataItem(QScriptEngine *engine) : QScriptClass(engine)
{
	ScriptEngineData::data(engine)->dataItem = this;
	qScriptRegisterMetaType(engine, dataItemToScriptValue, dataItemFromScriptValue);
	qScriptRegisterSequenceMetaType<QList<DataItem> >(engine);
	m_prototype = engine->newObject(this);
	m_subitem = engine->toStringHandle(QLatin1String("subitem"));
	m_subitems = engine->toStringHandle(QLatin1String("subitems"));
	m_subitemFunc = engine->newFunction(scriptSubitem);
	m_subitemsFunc = engine->newFunction(scriptSubitems);
	QScriptValue ctor = engine->newFunction(createDateItem);
	engine->globalObject().setProperty(name(), ctor);
}

QScriptClass::QueryFlags ScriptDataItem::queryProperty(const QScriptValue &object,
                                                             const QScriptString &name,
                                                             QueryFlags flags, uint *id)
{
	Q_UNUSED(name);
	Q_UNUSED(object);
	Q_UNUSED(flags);
	Q_UNUSED(id);
	return HandlesReadAccess/* | HandlesWriteAccess*/;
}

QScriptValue ScriptDataItem::property(const QScriptValue &object, const QScriptString &name, uint id)
{
	Q_UNUSED(id);
	if (name == m_subitem)
		return m_subitemFunc;
	if (name == m_subitems)
		return m_subitemsFunc;
	DataItem *item = get_data_item(object);
	debug() << Q_FUNC_INFO << item << (object.objectId() == m_prototype.objectId());
	Q_ASSERT(item);
	QVariant data = item->property(name.toString().toUtf8());
	if (data.isNull()) {
		DataItem subitem = item->subitem(name.toString());
		if (!subitem.isNull())
			return engine()->toScriptValue(subitem);
		return engine()->undefinedValue();
	}
	return engine()->newVariant(data);
}

void ScriptDataItem::setProperty(QScriptValue &object, const QScriptString &name,
                                uint id, const QScriptValue &value)
{
	Q_UNUSED(id);
	DataItem *item = get_data_item(object);
	item->setProperty(name.toString().toUtf8(), value.toVariant());
}

QScriptValue::PropertyFlags ScriptDataItem::propertyFlags(const QScriptValue &object,
                                                         const QScriptString &name, uint id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(id);
	return 0;
}

QScriptClassPropertyIterator *ScriptDataItem::newIterator(const QScriptValue &object)
{
	return new ScriptDataItemPropertyIterator(object);
}

QScriptValue ScriptDataItem::prototype() const
{
	return m_prototype;
}

QString ScriptDataItem::name() const
{
	return QLatin1String("DataItem");
}
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ScriptDataItemPtr)

