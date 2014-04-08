/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "yandexnarod.h"
#include "yandexnarodauthorizator.h"
#include "yandexnaroduploadjob.h"
#include <qutim/actiongenerator.h>
#include <qutim/contact.h>
#include <qutim/message.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/settingslayer.h>
#include <qutim/configbase.h>
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

struct YandexNarodScope
{
    QNetworkAccessManager *networkManager;
    YandexNarodCookieJar *cookieJar;
    YandexNarodAuthorizator *authorizator;

    YandexNarodScope() : networkManager(new QNetworkAccessManager),
        cookieJar(new YandexNarodCookieJar(networkManager)),
        authorizator(new YandexNarodAuthorizator(networkManager))
    {}

    ~YandexNarodScope()
    {
        networkManager->deleteLater();
    }
};
static QScopedPointer<YandexNarodScope> scope;

YandexNarodPlugin::YandexNarodPlugin()
{

}

YandexNarodPlugin::~YandexNarodPlugin()
{

}

void YandexNarodPlugin::init()
{
    setInfo(QT_TRANSLATE_NOOP("Plugin", "Yandex.Disk"),
            QT_TRANSLATE_NOOP("Plugin", "Send files via Yandex.Disk"),
            PLUGIN_VERSION(0, 2, 1, 0));
    setCapabilities(Loadable);
    addAuthor(QLatin1String("sauron"));
    addAuthor(QLatin1String("euroelessar"));
    addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
              QT_TRANSLATE_NOOP("Task", "Author"),
              QLatin1String("alexey.prokhin@yandex.ru"));
    addAuthor(QLatin1String("boiler"));
    addExtension(QT_TRANSLATE_NOOP("Plugin", "Yandex.Disk"),
                 QT_TRANSLATE_NOOP("Plugin", "Send files via Yandex.Disk"),
                 new SingletonGenerator<YandexNarodFactory>(),
                 ExtensionIcon(""));
}

bool YandexNarodPlugin::load()
{
    m_settingsItem.reset(new QmlSettingsItem("yandexDisk",
                                             Settings::Plugin,
                                             QIcon(),
                                             QT_TRANSLATE_NOOP("Yandex", "Yandex.Disk")));
    Settings::registerItem(m_settingsItem.data());

    scope.reset(new YandexNarodScope()); //TODO remove fucking singletone
    connect(scope->authorizator, SIGNAL(needSaveCookies()), SLOT(saveCookies()));
    connect(scope->networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveCookies()));

    return true;
}

bool YandexNarodPlugin::unload()
{
    Settings::removeItem(m_settingsItem.data());
    m_settingsItem.reset(0);
    scope.reset();
    return false;
}

void YandexNarodPlugin::loadCookies()
{
    // Do nothing
}

void YandexNarodPlugin::saveCookies()
{
    Config config;
    config.beginGroup(QLatin1String("yandex"));
    config.setValue(QLatin1String("token"), scope->authorizator->token(), Config::Crypted);
}

YandexNarodFactory::YandexNarodFactory() :
    FileTransferFactory(tr("Yandex.Narod"), 0)
{
    setIcon(QIcon(":/icons/yandexnarodplugin.png"));
    foreach (Protocol *protocol, Protocol::all()) {
        foreach (Account *account, protocol->accounts())
            onAccountAdded(account);
        connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
                SLOT(onAccountAdded(qutim_sdk_0_3::Account*)));
    }
}

bool YandexNarodFactory::checkAbility(ChatUnit *unit)
{
    Q_ASSERT(unit);
    Status status = unit->account()->status();
    return status != Status::Offline && status != Status::Connecting;
}

bool YandexNarodFactory::startObserve(ChatUnit *unit)
{
    Q_ASSERT(unit);
    m_observedUnits.insert(unit->account(), unit);
    return true;
}

bool YandexNarodFactory::stopObserve(ChatUnit *unit)
{
    Q_ASSERT(unit);
    Observers::iterator itr = m_observedUnits.begin();
    while (itr != m_observedUnits.end()) {
        if (*itr == unit)
            itr = m_observedUnits.erase(itr);
        else
            ++itr;
    }
    return true;
}

FileTransferJob *YandexNarodFactory::create(ChatUnit *unit)
{
    return new YandexNarodUploadJob(unit, this);
}

QNetworkAccessManager *YandexNarodFactory::networkManager()
{
    return scope->networkManager;
}

YandexNarodAuthorizator *YandexNarodFactory::authorizator()
{
    return scope->authorizator;
}

void YandexNarodFactory::onAccountAdded(qutim_sdk_0_3::Account *account)
{
    connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
            SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
}

void YandexNarodFactory::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
    bool isOnline = status != Status::Offline && status != Status::Connecting;
    foreach (ChatUnit *unit, m_observedUnits.values(sender()))
        changeAvailability(unit, isOnline);
}

YandexRequest::YandexRequest(const QUrl &url)
    : QNetworkRequest(url)
{
    QByteArray token = scope->authorizator->token().toLatin1();
    setRawHeader("Authorization", "OAuth " + token);
    setRawHeader("Accept", "*/*");
}

QUTIM_EXPORT_PLUGIN(YandexNarodPlugin)

