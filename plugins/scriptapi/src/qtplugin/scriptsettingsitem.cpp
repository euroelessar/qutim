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

#include "scriptsettingsitem.h"
#include "scriptsettingswidget.h"
#include <qutim/settingslayer.h>
#include <QMetaType>
#include <QScriptEngine>

namespace qutim_sdk_0_3
{
struct ScriptSettingsItemData;

class ScriptSettingsGenerator : public ObjectGenerator
{
	Q_DISABLE_COPY(ScriptSettingsGenerator)
public:
	ScriptSettingsGenerator(ScriptSettingsItemData *d) : d(d) {}
	
protected:
	virtual QObject *generateHelper() const;
	virtual const QMetaObject *metaObject() const;
private:
	ScriptSettingsItemData *d;
};

struct ScriptSettingsItemData
{
	typedef QSharedPointer<ScriptSettingsItemData> Ptr;
	
	ScriptSettingsItemData() : type(Settings::Plugin), item(0), generator(this) {}
	static Ptr get(const QScriptValue &value)
	{
		return value.data().toVariant().value<Ptr>();
	}
	
	Settings::Type type;
	QIcon icon;
	LocalizedString text;
	SettingsItem *item;
	QScriptValue that;
	QScriptValue generatorFunc;
	ScriptSettingsGenerator generator;
	QScriptValue widgetGuard;
	QPointer<QWidget> widget;
};

QObject *ScriptSettingsGenerator::generateHelper() const
{
	if (d->widget)
		return d->widget.data();
	d->widgetGuard = d->generatorFunc.call(d->that);
	QWidget *widget = d->widgetGuard.toVariant().value<QWidget*>();
	d->widget = QPointer<QWidget>(widget);
	return widget;
}

const QMetaObject *ScriptSettingsGenerator::metaObject() const
{
	return &ScriptSettingsWidget::staticMetaObject;
}

ScriptSettingsItem::ScriptSettingsItem(QScriptEngine *engine) : QScriptClass(engine)
{
	m_type = engine->toStringHandle(QLatin1String("type"));
	m_text = engine->toStringHandle(QLatin1String("text"));
	m_icon = engine->toStringHandle(QLatin1String("icon"));
	m_generator = engine->toStringHandle(QLatin1String("generator"));
	m_prototype = engine->newObject();
}

QScriptClass::QueryFlags ScriptSettingsItem::queryProperty(const QScriptValue &object,
                                                      const QScriptString &name,
                                                      QueryFlags flags, uint *id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(flags);
	Q_UNUSED(id);
	return HandlesReadAccess | HandlesWriteAccess;
}

QScriptValue ScriptSettingsItem::property(const QScriptValue &object, const QScriptString &name, uint id)
{
	Q_UNUSED(id);
	ScriptSettingsItemData::Ptr data = ScriptSettingsItemData::get(object);
	if (name == m_type)
		return data->type;
	else if (name == m_text)
		return engine()->newVariant(qVariantFromValue(data->text));
	else if (name == m_icon)
		return engine()->newVariant(qVariantFromValue(data->icon));
	else if (name == m_generator)
		return data->generatorFunc;
	return engine()->undefinedValue();
}

void ScriptSettingsItem::setProperty(QScriptValue &object, const QScriptString &name,
                                uint id, const QScriptValue &value)
{
	Q_UNUSED(id);
	ScriptSettingsItemData::Ptr data = ScriptSettingsItemData::get(object);
	if (name == m_type)
		data->type = static_cast<Settings::Type>(value.toInt32());
	else if (name == m_text)
		data->text = value.toVariant().value<LocalizedString>();
	else if (name == m_icon)
		data->icon = value.toVariant().value<QIcon>();
	else if (name == m_generator)
		data->generatorFunc = value;
}

QScriptValue::PropertyFlags ScriptSettingsItem::propertyFlags(const QScriptValue &object,
                                                         const QScriptString &name, uint id)
{
	Q_UNUSED(object);
	Q_UNUSED(name);
	Q_UNUSED(id);
	return 0;
}

QScriptClassPropertyIterator *ScriptSettingsItem::newIterator(const QScriptValue &object)
{
	Q_UNUSED(object);
//	return new ScriptSettingsItemPropertyIterator(object, msg);
	return 0;
}

QScriptValue ScriptSettingsItem::prototype() const
{
	return m_prototype;
}

QString ScriptSettingsItem::name() const
{
	return QLatin1String("SettingsItem");
}
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ScriptSettingsItemData::Ptr)

