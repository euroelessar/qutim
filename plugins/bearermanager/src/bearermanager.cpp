/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2014 Roman Tretyakov <roman@trett.ru>
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

#include "bearermanager.h"
#include "accountserver.h"

#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>

#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/utils.h>
#include <qutim/notification.h>
#include <qutim/accountmanager.h>

#define BEARER_PROPERTY "qutim_bearer_server"

using namespace qutim_sdk_0_3;

BearerManager::BearerManager() :
    m_confManager(new QNetworkConfigurationManager(this)), m_isOnline(false)
{
}

void BearerManager::init()
{
	setInfo(QT_TRANSLATE_NOOP("Service", "BearerManager"),
			QT_TRANSLATE_NOOP("Service", "Connection manager"),
            PLUGIN_VERSION(0, 0, 2, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("Sauron"));
	addAuthor(QLatin1String("trett"));
    addAuthor(QLatin1String("euroelessar"));
}

bool BearerManager::load()
{
	m_isOnline = m_confManager->isOnline();

    auto onAccountAdded = [this] (Account *account) {
        Q_ASSERT(account->property(BEARER_PROPERTY).isNull());
        Bearer::AccountServer *server = new Bearer::AccountServer(account);
        connect(this, &BearerManager::onlineStateChanged,
                server, &Bearer::AccountServer::setOnline);
        account->setProperty(BEARER_PROPERTY, QVariant::fromValue(server));
        server->setOnline(isNetworkOnline());
    };

    AccountManager *manager = AccountManager::instance();
    connect(manager, &AccountManager::accountAdded, this, onAccountAdded);
    connect(manager, &AccountManager::accountRemoved, this, [this] (Account *account) {
        auto server = account->property(BEARER_PROPERTY).value<Bearer::AccountServer *>();
        Q_ASSERT(server);
        account->setProperty(BEARER_PROPERTY, QVariant());
        delete server;
    });

    foreach (Account *account, manager->accounts())
        onAccountAdded(account);

	connect(m_confManager, SIGNAL(onlineStateChanged(bool)), SLOT(onOnlineStatusChanged(bool)));

	QList<QNetworkConfiguration> list = m_confManager->allConfigurations();
	if (!list.count()) {
		Notification::send(tr("Unable to find any network configuration. "
							  "Perhaps Qt or QtMobility network bearer configured incorrectly. "
                              "Bearer manager will not work properly, refer to your distribution maintainer."));
	}
	return true;
}

bool BearerManager::unload()
{
	return true;
}

void BearerManager::onOnlineStatusChanged(bool isOnline)
{
	if (m_isOnline == isOnline)
		return;
    m_isOnline = isOnline;

    qDebug() << "onlineStatusChanged, online:" << isOnline << ", network online:" << isNetworkOnline();

    emit onlineStateChanged(isNetworkOnline());
}

bool BearerManager::isNetworkOnline() const
{
	return m_confManager->isOnline()
			// We don't have any bearer backend
            || m_confManager->allConfigurations().isEmpty();
}

QUTIM_EXPORT_PLUGIN(BearerManager)

