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

#include "scriptpluginwrapper.h"
#include "scriptengine.h"
#include <qutim/thememanager.h>
#include <qutim/debug.h>
#include <QFile>
#include <QTextStream>

using namespace qutim_sdk_0_3;

static const uint qutim_meta_data_ScriptPlugin_hook[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

ScriptPluginWrapper::ScriptPluginWrapper(const QString &name) : 
		m_name(name), m_engine(0), m_apiInited(false)
{
	QMetaObject *meta = new QMetaObject;
	meta->d.superdata = &ScriptPluginWrapper::staticMetaObject;
	meta->d.data = qutim_meta_data_ScriptPlugin_hook;
	QByteArray stringdata = "ScriptPluginWrapper::";
	stringdata += name.toUtf8();
	stringdata += '\0';
	char *tmpPtr = reinterpret_cast<char*>(qMalloc(stringdata.size() + 1));
	qMemCopy(tmpPtr, stringdata.constData(), stringdata.size() + 1);
	meta->d.stringdata = tmpPtr;
	meta->d.extradata = 0;
	QObject::d_ptr->metaObject = meta;
	debug() << metaObject()->className() << this;
}

ScriptPluginWrapper::~ScriptPluginWrapper()
{
	QObject::d_ptr->metaObject = 0;
}

void ScriptPluginWrapper::init()
{
	m_engine = new ScriptEngine(m_name, this);
	QFile scriptFile(ThemeManager::path(QLatin1String("scripts"), m_name) + QLatin1String("/plugin.js"));
	debug() << Q_FUNC_INFO << scriptFile.fileName();
	if (!scriptFile.open(QIODevice::ReadOnly))
		return;
	QTextStream stream(&scriptFile);
	m_engine->evaluate(stream.readAll());
	debug() << m_engine->uncaughtException().toString() << m_engine->uncaughtExceptionLineNumber();
	QScriptValue plugin = m_engine->globalObject().property("plugin");
	setInfo(qscriptvalue_cast<LocalizedString>(plugin.property("name")),
			qscriptvalue_cast<LocalizedString>(plugin.property("description")),
			plugin.property("value").toInt32(),
			ExtensionIcon(plugin.property("icon").toString()));
	setCapabilities(Loadable);
	QScriptValue authors = plugin.property("authors");
	int length = authors.property(QLatin1String("length")).toInt32();
	for (int i = 0; i < length; i++) {
		QScriptValue author = authors.property(i);
		addAuthor(qscriptvalue_cast<LocalizedString>(author.property("name")),
				  qscriptvalue_cast<LocalizedString>(author.property("task")),
				  author.property("email").toString(),
				  author.property("web").toString());
	}		
}

bool ScriptPluginWrapper::load()
{
	if (!m_apiInited) {
		m_apiInited = true;
		m_engine->initApi();
	}
	QScriptValue plugin = m_engine->globalObject().property("plugin");
	QScriptValue loadFunc = plugin.property("load");
	bool result = loadFunc.call(plugin).toBool();
	debug() << m_engine->uncaughtException().toString() << m_engine->uncaughtExceptionLineNumber();
	return result;
}

bool ScriptPluginWrapper::unload()
{
	QScriptValue plugin = m_engine->globalObject().property("plugin");
	QScriptValue unloadFunc = plugin.property("unload");
	bool result = unloadFunc.call(plugin).toBool();
	debug() << m_engine->uncaughtException().toString() << m_engine->uncaughtExceptionLineNumber();
	return result;
}

