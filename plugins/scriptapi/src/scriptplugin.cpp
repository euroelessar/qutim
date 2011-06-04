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

#include "scriptplugin.h"
#include "scriptengine.h"
#include "scriptpluginwrapper.h"
#include <QDebug>
#include <qutim/thememanager.h>

ScriptHelperWidget::ScriptHelperWidget()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	m_textEdit = new QPlainTextEdit(this);
	QPushButton *button = new QPushButton(this);
	layout->addWidget(m_textEdit);
	layout->addWidget(button);
	connect(button, SIGNAL(clicked()), SLOT(onButtonClicked()));
    m_engine.importExtension(QLatin1String("qt.core"));
	m_engine.importExtension(QLatin1String("qt.gui"));
	m_engine.importExtension(QLatin1String("qutim"));
}

void ScriptHelperWidget::onButtonClicked()
{
	m_engine.evaluate(m_textEdit->toPlainText());
	if (m_engine.hasUncaughtException()) {
		qDebug() << m_engine.uncaughtException().toString();
		qDebug() << m_engine.uncaughtExceptionBacktrace().join("\n");
	}
}

ScriptPlugin::ScriptPlugin()
{
}

void ScriptPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Developer"),
			  QLatin1String("euroelessar@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Script API"),
			QT_TRANSLATE_NOOP("Plugin", "Added ability to create plugins at JavaScript"),
			PLUGIN_VERSION(0, 0, 1, 0), ExtensionIcon("network-wireless"));
	setCapabilities(Loadable);
}

bool ScriptPlugin::load()
{
	qDebug() << Q_FUNC_INFO << ThemeManager::list("scripts");
	ScriptHelperWidget *widget = new ScriptHelperWidget;
	widget->show();
	return true;
}

QList<Plugin*> ScriptPlugin::loadPlugins()
{
	QList<Plugin*> plugins;
	foreach (const QString &name, ThemeManager::list("scripts"))
		plugins << new ScriptPluginWrapper(name);
	return plugins;
}

//void ScriptPlugin::evaluate()
//{
//	qDebug("%s", Q_FUNC_INFO);
//	QFile scriptFile("/home/elessar/.local/share/qutim/scripts/test/plugin.json");
//	if (!scriptFile.open(QIODevice::ReadOnly))
//		return;
//	QTextStream stream(&scriptFile);
//	m_engine->evaluate(stream.readAll());
//	QScriptValue plugin = m_engine->globalObject().property("plugin");
//	QScriptValue loadFunc = plugin.property("load");
//	return loadFunc.call(plugin).toBool();
////			"client.settings.value = {\n"
////			"'test': 123,\n"
////			"'test2': '323'\n"
////			"};\n"
////			"var config = client.settings.value;"
////			"client.debug(config.test, config.test2);\n");
////	"client.settings.register({\n"
////	"	'type': client.settings.General,\n"
////	"	'text': QT_TRANSLATE_NOOP('123', 'Kak dela?'),\n"
////	"	'entries': [\n"
////	"		{\n"
////	"			'type': 'LineEdit',\n"
////	"			'name': 'justText',\n"
////	"			'text': 'original text'\n"
////	"		}\n"
////	"	]\n"
////	"});\n"
////						"var account = client.protocols.jabber.account('euroelessar@qutim.org');\n"
////					   "var status = account.status;\n"
////					   "status.text = 'Привет от QtScript!';\n"
////					   "status.type = client.status.Online;\n"
////					   "client.debug(status.type);\n"
////					   "account.status = status;\n"
////					   "var sauron = account.unit('sauron@qutim.org');\n"
////					   "function onSessionCreated(session)\n"
////					   "{\n"
////					   "	client.debug('Started chat session with', session.unit().id);\n"
////					   "}\n"
////					   "client.chatLayer.sessionCreated.connect(onSessionCreated);\n"
////					   "client.debug(sauron.id, sauron.title);");
////	"function sendMessageToSauron()\n"
////	"{\n"
////	"	client.debug('Send message');"
////	"	sauron.sendMessage('Привет от QtScript!');\n"
////	"}\n"
////	"sauron.statusChanged.connect(sendMessageToSauron);\n"
////	'euroelessar@qutim.org').id
//	qDebug("%s %d", qPrintable(m_engine->uncaughtException().toString()), m_engine->uncaughtExceptionLineNumber());
////	'euroelessar@qutim.org'
//}

bool ScriptPlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(ScriptPlugin)
