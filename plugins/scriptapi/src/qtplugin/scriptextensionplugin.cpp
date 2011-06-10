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

#include "scriptextensionplugin.h"
#include "scriptmessage.h"
#include "scriptenginedata.h"
#include "scriptservices.h"
#include "scriptsettingswidget.h"
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/conference.h>
#include <qutim/notificationslayer.h>
#include <qutim/settingslayer.h>
#include <qutim/chatsession.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QScriptEngine>
#include <QScriptContext>

template <>
void qScriptValueToSequence<QSet<QString> >(const QScriptValue &value, QSet<QString> &cont)
{
    quint32 len = value.property(QLatin1String("length")).toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
		cont.insert(item.toString());
	}
}

namespace qutim_sdk_0_3
{
template <typename T>
QScriptValue qobjectToScriptValue(QScriptEngine *engine, const T &t)
{
	return engine->newQObject(t);
}

template <typename T>
void qobjectFromScriptValue(const QScriptValue &obj, T &t)
{
	t = qobject_cast<T>(obj.toQObject());
}

template <typename T>
int scriptRegisterQObject(QScriptEngine *engine)
{
	int type = qScriptRegisterMetaType<T*>(engine, qobjectToScriptValue, qobjectFromScriptValue);
	qScriptRegisterSequenceMetaType<QList<T*> >(engine);
	return type;
}

QScriptValue getProtocol(QScriptContext *context, QScriptEngine *engine)
{
	if (context->argumentCount() < 1)
		return engine->undefinedValue();
	QString id = context->argument(0).toString();
	Protocol *protocol = Protocol::all().value(id);
	if (!protocol)
		return engine->undefinedValue();
	return engine->newQObject(protocol);
}

QScriptValue scriptSettingsRegister(QScriptContext *ctxt, QScriptEngine *e)
{
	Q_UNUSED(ctxt);
	Q_UNUSED(e);
	//	if (ctxt->argumentCount() != 1)
	//		return ctxt->throwError("register() takes exactly one argument");
	//	QScriptValue item = ctxt->argument(0);
	//	Settings::Type itemType = static_cast<Settings::Type>(item.property("type").toInt32());
	//	QString icon = item.property("icon").toString();
	//	LocalizedString text = qscriptvalue_cast<LocalizedString>(item.property("text"));
	//	AutoSettingsItem *settingsItem = new AutoSettingsItem(itemType, Icon(icon), text);
	//	settingsItem->setConfig(QString(), QLatin1String("plugins/script/")
	//							+ static_cast<ScriptEngine*>(e)->name());
	//	QScriptValue onSaved = item.property("onSaved");
	//	if (onSaved.isFunction()) {
	//		ScriptSettingsConnector *helper = new ScriptSettingsConnector(e);
	//		settingsItem->connect(SIGNAL(saved()), helper, SLOT(onSaved()));
	//		qScriptConnect(helper, SIGNAL(saved()), item, onSaved);
	//	}
	
	//	QScriptValue entries = item.property("entries");
	//    quint32 len = entries.property(QLatin1String("length")).toUInt32();
	//    for (quint32 i = 0; i < len; ++i) {
	//        QScriptValue entry = entries.property(i);
	//		text = qscriptvalue_cast<LocalizedString>(entry.property("text"));
	//		ObjectGenerator *gen = 0;
	//		QString type = entry.property("type").toString();
	//		if (type == QLatin1String("CheckBox"))
	//			gen = new GeneralGenerator<QCheckBox>();
	//		else if (type == QLatin1String("ComboBox"))
	//			gen = new GeneralGenerator<AutoSettingsComboBox>();
	//		else if (type == QLatin1String("LineEdit"))
	//			gen = new GeneralGenerator<QLineEdit>();
	//		else if (type == QLatin1String("TextEdit"))
	//			gen = new GeneralGenerator<QTextEdit>();
	//		else if (type == QLatin1String("PlainTextEdit"))
	//			gen = new GeneralGenerator<QPlainTextEdit>();
	//		else
	//			continue;
	//		AutoSettingsItem::Entry *entryItem = settingsItem->addEntry(text, gen);
	//		QScriptValueIterator it(entry);
	//		while (it.hasNext()) {
	//			it.next();
	//			if (it.name() == QLatin1String("type") || it.name() == QLatin1String("text"))
	//				continue;
	//			else if (it.name() == QLatin1String("name"))
	//				entryItem->setName(it.value().toString());
	//			else
	//				entryItem->setProperty(it.name().toUtf8().constData(), it.value().toVariant());
	//		}
	//	}
	//	Settings::registerItem(settingsItem);
	return QScriptValue();
}

QScriptValue scriptSettingsRemove(QScriptContext *ctxt, QScriptEngine *e)
{
	Q_UNUSED(ctxt);
	return e->undefinedValue();
}

QScriptValue createSettingsWidget(QScriptContext *, QScriptEngine *engine)
{
	ScriptSettingsWidget *widget = new ScriptSettingsWidget;
	return engine->newQObject(widget);
}

ScriptExtensionPlugin::ScriptExtensionPlugin(QObject *parent)
    : QScriptExtensionPlugin(parent)
{
	qDebug() << Q_FUNC_INFO;
}

QStringList ScriptExtensionPlugin::keys() const
{
	return QStringList(QLatin1String("qutim"));
}

void ScriptExtensionPlugin::initialize(const QString &key, QScriptEngine *engine)
{
	if (key != QLatin1String("qutim"))
		return;
	QScriptValue qutim = setupPackage(QLatin1String("qutim"), engine);
	qScriptRegisterSequenceMetaType<QSet<QString> >(engine);
	scriptRegisterQObject<Account>(engine);
	scriptRegisterQObject<ChatUnit>(engine);
	scriptRegisterQObject<Buddy>(engine);
	scriptRegisterQObject<Contact>(engine);
	scriptRegisterQObject<Conference>(engine);
	scriptRegisterQObject<ChatSession>(engine);
	ScriptEngineData *data = ScriptEngineData::data(engine);
	data->message = new ScriptMessage(engine);
	data->services = new ScriptServices(engine);
	qutim.setProperty(QLatin1String("services"), engine->newObject(data->services));
	qutim.setProperty(QLatin1String("protocol"), engine->newFunction(getProtocol, 1));
	{
		QScriptValue settings = engine->newObject();
		settings.setProperty(QLatin1String("General"), Settings::General);
		settings.setProperty(QLatin1String("Protocol"), Settings::Protocol);
		settings.setProperty(QLatin1String("Appearance"), Settings::Appearance);
		settings.setProperty(QLatin1String("Plugin"), Settings::Plugin);
		settings.setProperty(QLatin1String("Special"), Settings::Special);
		settings.setProperty(QLatin1String("register"), engine->newFunction(scriptSettingsRegister, 1));
		settings.setProperty(QLatin1String("remove"), engine->newFunction(scriptSettingsRemove, 1));
		qutim.setProperty(QLatin1String("settings"), settings);
	}
	engine->globalObject().setProperty(QLatin1String("SettingsWidget"),
	                                   engine->newFunction(createSettingsWidget));
}
}

static class QtScript_qutim_0_3_PluginInstance
{
public:
	QtScript_qutim_0_3_PluginInstance()
	{
		qRegisterStaticPluginInstanceFunction(createInstance);
	}
	
	static QObject *createInstance()
	{
		static QWeakPointer<QObject> instance;
		if (!instance)
			instance = QWeakPointer<QObject>(new qutim_sdk_0_3::ScriptExtensionPlugin);
		return instance.data();
	}
} static_qtscript_qutim_0_3_PluginInstance;

Q_DECLARE_METATYPE(QSet<QString>)
