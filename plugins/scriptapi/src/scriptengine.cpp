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

#include "scriptengine.h"
#include "scriptmessageclass.h"
#include "scriptsettingsconnector.h"
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/conference.h>
#include <qutim/message.h>
#include <qutim/chatsession.h>
#include <qutim/notification.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QScriptValueIterator>
#include <QDebug>
#include <QMetaEnum>

using namespace qutim_sdk_0_3;

QScriptValue scriptConsoleLog(QScriptContext *ctxt, QScriptEngine *e)
{
	Q_UNUSED(e);
	QString message;
	for (int i = 0; i < ctxt->argumentCount(); i++) {
		if (!message.isEmpty())
			message += ' ';
		message += ctxt->argument(i).toString();
	}
	qDebug("%s", qPrintable(message));
	return QScriptValue();
}

template <typename T>
QScriptValue objectToScriptValue(QScriptEngine *engine, const T &t)
{
	return engine->newQObject(t);
}

template <typename T>
void objectFromScriptValue(const QScriptValue &obj, T &t)
{
	t = qobject_cast<T>(obj.toQObject());
}

template <typename T>
inline int scriptRegisterMetaType(QScriptEngine *engine)
{
	int type = qScriptRegisterMetaType<T*>(engine, objectToScriptValue, objectFromScriptValue);
	qScriptRegisterSequenceMetaType<QList<T*> >(engine);
	return type;
}

QScriptValue messageToScriptValue(QScriptEngine *engine, const Message &mes)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("time", engine->newDate(mes.time()));
	obj.setProperty("chatUnit", engine->newQObject(const_cast<ChatUnit *>(mes.chatUnit())));
	obj.setProperty("text", mes.text());
	obj.setProperty("in", mes.isIncoming());
	foreach(const QByteArray &name, mes.dynamicPropertyNames())
		obj.setProperty(QString::fromUtf8(name), engine->newVariant(mes.property(name)));
	return obj;
}

void messageFromScriptValue(const QScriptValue &obj, Message &mes)
{
	QScriptValueIterator it(obj);
	while (it.hasNext()) {
		it.next();
		mes.setProperty(it.name().toUtf8(), it.value().toVariant());
	}
}

QScriptValue messagePtrToScriptValue(QScriptEngine *engine, Message * const &mes)
{
	QScriptValue data = engine->newVariant(qVariantFromValue(mes));
	QScriptValue obj = engine->newObject(new ScriptMessageClass(engine), data);
	return obj;
}

void messagePtrFromScriptValue(const QScriptValue &obj, Message * &mes)
{
	mes = obj.data().toVariant().value<Message*>();
}

QScriptValue statusGetSetType(QScriptContext *ctx, QScriptEngine *eng)
{
	QScriptValue obj = ctx->thisObject();
	QScriptValue data = obj.data();
	if (!data.isValid()) {
		data = eng->newObject();
		obj.setData(data);
	}
	QScriptValue result;
	if (ctx->argumentCount() == 1) {
		qint32 type = ctx->argument(0).toInt32();
		data.setProperty("type", type);
		obj.setProperty("subtype", 0);
		Status status(static_cast<Status::Type>(type));
		obj.setProperty("name", status.name().toString());
		result = type;
	} else {
		result = data.property("type");
	}
	return result;
}

QScriptValue localizedStringToString(QScriptContext *ctxt, QScriptEngine *)
{
	QScriptValue value = ctxt->thisObject();
	return qscriptvalue_cast<LocalizedString>(value).toString();
}

QScriptValue localizedStringToScriptValue(QScriptEngine *engine, const LocalizedString &s)
{
	QScriptValue value = engine->newObject();
	value.setProperty("context", QString::fromUtf8(s.context()));
	value.setProperty("text", QString::fromUtf8(s.original()));
	value.setProperty("toString", engine->newFunction(localizedStringToString));
	return value;
}

void localizedStringFromScriptValue(const QScriptValue &obj, LocalizedString &s)
{
	s.setContext(obj.property("context").toString().toUtf8());
	s.setOriginal(obj.property("text").toString().toUtf8());
}

QScriptValue statusToScriptValue(QScriptEngine *engine, const Status &s)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("text", s.text());
	obj.setProperty("name", s.name().toString());
	obj.setProperty("type", engine->newFunction(statusGetSetType),
					QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
	obj.setProperty("type", s.type());
	obj.setProperty("subtype", s.subtype());
//	obj.setProperty("extendedInfos", s.extendedInfos());
//	foreach(const QByteArray &name, s.dynamicPropertyNames())
//		obj.setProperty(QString::fromUtf8(name), engine->newVariant(s.property(name)));
	return obj;
}

void statusFromScriptValue(const QScriptValue &obj, Status &s)
{
	QScriptValueIterator it(obj);
	while (it.hasNext()) {
		it.next();
		s.setProperty(it.name().toUtf8(), it.value().toVariant());
	}
}

QScriptValue notificationsSend(QScriptContext *ctxt, QScriptEngine *e)
{
	if (ctxt->argumentCount() < 1)
		return e->undefinedValue();
	QScriptValue arg = ctxt->argument(0);
	if (arg.isNumber() && ctxt->argumentCount() > 1) {
		NotificationRequest request;
		request.setType(static_cast<Notification::Type>(arg.toInt32()));
		request.setObject(ctxt->argument(1).toQObject());
		if (ctxt->argumentCount() > 2)
			request.setText(ctxt->argument(2).toString());
		return e->newQObject(request.send());
	} else if (arg.isString()) {
		return e->newQObject(Notification::send(arg.toString()));
	} else if (arg.isObject()) {
		Message msg = qscriptvalue_cast<Message>(arg);
		return e->newQObject(Notification::send(msg));
	}
	return e->undefinedValue();
}

QScriptValue translateNoopHook(QScriptContext *ctxt, QScriptEngine *e)
{
	if (ctxt->argumentCount() != 2)
		return ctxt->throwError("QT_TRANSLATE_NOOP() takes exactly two arguments");
	QScriptValue value = e->newObject();
	value.setProperty("context", ctxt->argument(0));
	value.setProperty("text", ctxt->argument(1));
	return value;
}

QScriptValue scriptSettingsRegister(QScriptContext *ctxt, QScriptEngine *e)
{
	if (ctxt->argumentCount() != 1)
		return ctxt->throwError("register() takes exactly one argument");
	QScriptValue item = ctxt->argument(0);
	Settings::Type itemType = static_cast<Settings::Type>(item.property("type").toInt32());
	QString icon = item.property("icon").toString();
	LocalizedString text = qscriptvalue_cast<LocalizedString>(item.property("text"));
	AutoSettingsItem *settingsItem = new AutoSettingsItem(itemType, Icon(icon), text);
	settingsItem->setConfig(QString(), QLatin1String("plugins/script/")
							+ static_cast<ScriptEngine*>(e)->name());
	QScriptValue onSaved = item.property("onSaved");
	if (onSaved.isFunction()) {
		ScriptSettingsConnector *helper = new ScriptSettingsConnector(e);
		settingsItem->connect(SIGNAL(saved()), helper, SLOT(onSaved()));
		qScriptConnect(helper, SIGNAL(saved()), item, onSaved);
	}
	
	QScriptValue entries = item.property("entries");
    quint32 len = entries.property(QLatin1String("length")).toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue entry = entries.property(i);
		text = qscriptvalue_cast<LocalizedString>(entry.property("text"));
		ObjectGenerator *gen = 0;
		QString type = entry.property("type").toString();
		if (type == QLatin1String("CheckBox"))
			gen = new GeneralGenerator<QCheckBox>();
		else if (type == QLatin1String("ComboBox"))
			gen = new GeneralGenerator<AutoSettingsComboBox>();
		else if (type == QLatin1String("LineEdit"))
			gen = new GeneralGenerator<QLineEdit>();
		else if (type == QLatin1String("TextEdit"))
			gen = new GeneralGenerator<QTextEdit>();
		else if (type == QLatin1String("PlainTextEdit"))
			gen = new GeneralGenerator<QPlainTextEdit>();
		else
			continue;
		AutoSettingsItem::Entry *entryItem = settingsItem->addEntry(text, gen);
		QScriptValueIterator it(entry);
		while (it.hasNext()) {
			it.next();
			if (it.name() == QLatin1String("type") || it.name() == QLatin1String("text"))
				continue;
			else if (it.name() == QLatin1String("name"))
				entryItem->setName(it.value().toString());
			else
				entryItem->setProperty(it.name().toUtf8().constData(), it.value().toVariant());
		}
	}
	Settings::registerItem(settingsItem);
	return QScriptValue();
}

QScriptValue scriptSettingsRemove(QScriptContext *ctxt, QScriptEngine *e)
{
	Q_UNUSED(ctxt);
	return e->undefinedValue();
}

QVariant scriptValueToVariant(const QScriptValue &value)
{
	QVariant var;
	if (value.isBool() || value.isNumber()
		|| value.isString() || value.isVariant()
		|| value.isDate() || value.isRegExp()) {
		var = value.toVariant();
	} else if (value.isArray()) {
		QVariantList list;
		int len = value.property(QLatin1String("length")).toInt32();
		for (int i = 0; i < len; i++)
			list << scriptValueToVariant(value.property(i));
		var = list;
	} else if (value.isObject()) {
		QVariantMap map;
		QScriptValueIterator it(value);
		while (it.hasNext()) {
			it.next();
			map.insert(it.name(), scriptValueToVariant(it.value()));
		}
		var = map;
	}
	return var;
}

QScriptValue variantToScriptValue(const QVariant &var, QScriptEngine *e)
{
	QScriptValue value;
	switch (var.type()) {
	case QVariant::Map: {
		value = e->newObject();
		const QVariantMap map = var.toMap();
		for (QVariantMap::const_iterator it = map.begin(); it != map.end(); it++)
			value.setProperty(it.key(), variantToScriptValue(it.value(), e));
		break;
	}
	case QVariant::List: {
		const QVariantList list = var.toList();
		value = e->newArray(list.size());
		for (int i = 0; i < list.size(); i++)
			value.setProperty(i, variantToScriptValue(list.at(i), e));
		break;
	}
	default:
		value = e->newVariant(var);
	}
	return value;
}

QScriptValue settingsGetSetValue(QScriptContext *ctxt, QScriptEngine *e)
{
	QScriptValue value;
	Config cfg;
	cfg.beginGroup("plugins/script");
	if (ctxt->argumentCount() == 1) { // writing?
		QScriptValue value = ctxt->argument(0);
		cfg.setValue(static_cast<ScriptEngine*>(e)->name(), scriptValueToVariant(value));
	} else {
		QVariant var = cfg.value(static_cast<ScriptEngine*>(e)->name());
		value = variantToScriptValue(var, e);
	}
	return value;
}

ScriptEngine::ScriptEngine(const QString &name, QObject *parent) :
		QScriptEngine(parent), m_name(name)
{
	connect(this, SIGNAL(signalHandlerException(QScriptValue)),
			this, SLOT(onException(QScriptValue)));
	scriptRegisterMetaType<Account>(this);
	scriptRegisterMetaType<ChatUnit>(this);
	scriptRegisterMetaType<Buddy>(this);
	scriptRegisterMetaType<Contact>(this);
	scriptRegisterMetaType<Conference>(this);
	scriptRegisterMetaType<ChatSession>(this);
	qScriptRegisterMetaType(this, messageToScriptValue, messageFromScriptValue);
	qScriptRegisterMetaType(this, messagePtrToScriptValue, messagePtrFromScriptValue);
	qScriptRegisterMetaType(this, statusToScriptValue, statusFromScriptValue);
	qScriptRegisterMetaType(this, localizedStringToScriptValue, localizedStringFromScriptValue);
	installTranslatorFunctions();
	globalObject().setProperty("QT_TRANSLATE_NOOP", newFunction(translateNoopHook));
	
	QScriptValue client = newObject();
	client.setProperty("log", newFunction(scriptConsoleLog, 1));
	client.setProperty("debug", newFunction(scriptConsoleLog, 1));
	globalObject().setProperty("client", client);
}

void ScriptEngine::initApi()
{
	QScriptValue client = globalObject().property("client");
	foreach (const QByteArray &name, ServiceManager::names()) {
		QScriptValue service = newQObject(ServiceManager::getByName(name));
		QString prop = QString::fromUtf8(name.data(), name.size());
		if (prop.isEmpty())
			continue;
		prop[0] = prop.at(0).toLower();
		client.setProperty(prop, service);
	}
	{
		QScriptValue settings = newObject();
		settings.setProperty("General", Settings::General);
		settings.setProperty("Protocol", Settings::Protocol);
		settings.setProperty("Appearance", Settings::Appearance);
		settings.setProperty("Plugin", Settings::Plugin);
		settings.setProperty("Special", Settings::Special);
		settings.setProperty("register", newFunction(scriptSettingsRegister, 1));
		settings.setProperty("remove", newFunction(scriptSettingsRemove, 1));
		settings.setProperty("value", newFunction(settingsGetSetValue, 1),
							 QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
		client.setProperty("settings", settings);
	}
	{
		QScriptValue status = newObject();
		status.setProperty("Online", Status::Online);
		status.setProperty("FreeChat", Status::FreeChat);
		status.setProperty("Away", Status::Away);
		status.setProperty("NA", Status::NA);
		status.setProperty("DND", Status::DND);
		status.setProperty("Invisible", Status::Invisible);
		status.setProperty("Offline", Status::Offline);
		client.setProperty("status", status);
	}
	{
		QScriptValue notifications = newObject();
		QMetaObject meta = Notification::staticMetaObject;
		for (int i = 0; i < meta.enumeratorCount(); ++i) {
			QMetaEnum e = meta.enumerator(i);
			if (e.name() == QLatin1String("Type")) {
				for (int j = 0;j != e.keyCount(); j++)
					notifications.setProperty(e.key(j), e.value(j));
			}
		}

		//O-o
		//notifications.setProperty("Online", Notifications::Online);
		//notifications.setProperty("Offline", Notifications::Offline);
		//notifications.setProperty("StatusChange", Notifications::StatusChange);
		//notifications.setProperty("Birthday", Notifications::Birthday);
		//notifications.setProperty("Startup", Notifications::Startup);
		//notifications.setProperty("MessageGet", Notifications::MessageGet);
		//notifications.setProperty("MessageSend", Notifications::MessageSend);
		//notifications.setProperty("System", Notifications::System);
		//notifications.setProperty("Typing", Notifications::Typing);
		//notifications.setProperty("BlockedMessage", Notifications::BlockedMessage);

		notifications.setProperty("send", newFunction(notificationsSend, 1));
		client.setProperty("notifications", notifications);
	}
	{
		QScriptValue protocols = newObject();
		ProtocolHash allProtos = Protocol::all();
		ProtocolHash::const_iterator it = allProtos.constBegin();
		for (; it != allProtos.constEnd(); it++) {
			QScriptValue protocol = newQObject(it.value());
			protocols.setProperty(it.key(), protocol);
		}
		client.setProperty("protocols", protocols);
	}
}

void ScriptEngine::onException(const QScriptValue &exception)
{
	qDebug() << exception.toString();
	qDebug() << uncaughtExceptionBacktrace();
}

