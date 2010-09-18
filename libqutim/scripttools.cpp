#include "scripttools.h"
#include "message.h"
#include "protocol.h"
#include "servicemanager.h"

namespace qutim_sdk_0_3
{
	ScriptTools::ScriptTools()
	{
	}

	ScriptTools::~ScriptTools()
	{
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
		ProtocolMap map = Protocol::all();
		ProtocolMap::iterator it;
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
