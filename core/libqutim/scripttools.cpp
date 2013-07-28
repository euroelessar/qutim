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
#include "scripttools.h"
#include "message.h"
#include "protocol.h"
#include "servicemanager.h"

namespace qutim_sdk_0_3
{

Q_GLOBAL_STATIC(QScriptEngine, scriptEngineInstance)

ScriptTools::ScriptTools()
{
}

ScriptTools::~ScriptTools()
{
}

QScriptEngine *ScriptTools::engineInstance()
{
	return scriptEngineInstance();
}

void ScriptTools::prepareEngine(QScriptEngine *engine)
{
	QList<QByteArray> services = ServiceManager::names();
	QScriptValue client = engine->newObject();
	for (int i = 0; i < services.size(); i++) {
		QByteArray origName = services.at(i);
		Q_ASSERT(!origName.isEmpty());
		QString name = QChar(QLatin1Char(origName.at(0))).toLower();
		name += QLatin1String(origName.constData() + 1);
		client.setProperty(name, engine->newQObject(ServiceManager::getByName(origName),
		                                            QScriptEngine::QtOwnership,
		                                            QScriptEngine::AutoCreateDynamicProperties));
	}
	QScriptValue protocols = engine->newObject();
	ProtocolHash map = Protocol::all();
	ProtocolHash::iterator it;
	for (it = map.begin(); it != map.end(); it++)
		protocols.setProperty(it.key(), engine->newQObject(it.value()));
	client.setProperty("protocols", protocols);
	Message::scriptRegister(engine);
	engine->globalObject().setProperty("Client", client);
}

void ScriptTools::prepareEngine(QDeclarativeEngine *engine)
{
	Q_UNUSED(engine);
}
}

