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
	addAuthor(QLatin1String("sauron"));
}

bool VWallManager::load()
{
    ActionGenerator *gen = new ActionGenerator(QIcon(),
									QT_TRANSLATE_NOOP("Vkontakte","View wall"),
									this,
									SLOT(onViewWallTriggered(QObject*)));
    gen->setType(ActionTypeContactList);
    MenuController::addAction<VContact>(gen);
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

QUTIM_EXPORT_PLUGIN(Vkontakte::VWallManager);

