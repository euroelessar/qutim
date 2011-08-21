#include "vwallmanager.h"
#include <qutim/menucontroller.h>
#include "../../src/vcontact.h"
#include "vwallsession.h"
#include "../../src/vaccount.h"
#include <qutim/debug.h>

namespace Vkontakte
{

void VWallManager::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Wall"),
			QT_TRANSLATE_NOOP("Plugin", "Vkontakte wall support for qutIM"),
			PLUGIN_VERSION(0, 0, 1, 0));
	setCapabilities(Loadable);
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Sidorov"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me")
 			);
}

bool VWallManager::load()
{
	static ActionGenerator wall_gen(QIcon(),
									QT_TRANSLATE_NOOP("Vkontakte","View wall"),
									this,
									SLOT(onViewWallTriggered(QObject*)));
	wall_gen.setType(ActionTypeContactList);
	MenuController::addAction<VContact>(&wall_gen);
	return true;
}

bool VWallManager::unload()
{
	return false;
}

void VWallManager::onViewWallTriggered(QObject* obj)
{
	VContact *con = qobject_cast<VContact*>(obj);
	VAccount *acc = static_cast<VAccount*>(con->account());
	Q_ASSERT(obj);
	VWallSession *wall = m_sessions.value(acc).value(con->id());
	if (!wall) {
		wall = new VWallSession(con->id(),acc);
		m_sessions[con->account()].insert(con->id(),wall);
		connect(wall,SIGNAL(destroyed(QObject*)),SLOT(onWallDestroyed(QObject*)));
	}
	wall->join();
}

void VWallManager::onWallDestroyed(QObject* wall)
{
	VWallSession *session = reinterpret_cast<VWallSession*>(wall);
	QHash<QObject*, QHash<QString, VWallSession*> >::iterator it;
	for (it = m_sessions.begin();it!=m_sessions.end();it++) {
		QString key = it->key(session);
		if (!key.isEmpty())
			it->remove(key);
	}

}

}

QUTIM_EXPORT_PLUGIN(Vkontakte::VWallManager)
