#include "quickproxyhelper.h"
#include <qutim/account.h>
#include <qutim/protocol.h>

namespace MeegoIntegration {

using namespace qutim_sdk_0_3;

QuickProxyHelper::QuickProxyHelper(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<DataItem>();
}

QObject *QuickProxyHelper::account() const
{
    return m_account;
}

void QuickProxyHelper::setAccount(QObject *account)
{
    account = qobject_cast<Account*>(account);
    if (m_account != account) {
        m_account = static_cast<Account*>(account);
        emit accountChanged(account);
    }
    if (m_account) {
		NetworkProxyManager *manager = NetworkProxyManager::get(m_account->protocol());
		m_infos = manager ? manager->proxies() : QList<NetworkProxyInfo*>();
	} else {
		m_infos = NetworkProxyInfo::allProxies();
    }
}

Config QuickProxyHelper::config() const
{
    Config cfg = m_account ? m_account->config() : Config();
    return cfg.group(QLatin1String("proxy"));
}

int QuickProxyHelper::count()
{
    return m_infos.count();
}

QString QuickProxyHelper::name(int index)
{
    return m_infos.at(index)->name();
}

QString QuickProxyHelper::description(int index)
{
    return m_infos.at(index)->description();
}

QVariant QuickProxyHelper::settings(int index)
{
    return qVariantFromValue(m_infos.at(index)->settings(config()));
}

void QuickProxyHelper::save(const QVariant &itemData, int index, bool disabled, bool useGlobal)
{
    NetworkProxyInfo *proxy = m_infos.value(index);
    DataItem settings = itemData.value<DataItem>();
    Config globalCfg = Config().group("proxy");
	Config cfg = m_account ? m_account->config("proxy") : globalCfg;
    cfg.setValue("disabled", disabled);
    cfg.setValue("useGlobal", useGlobal);
    if (!disabled && proxy)
        proxy->saveSettings(cfg, settings);
    
    if (proxy && settings.subitem("type").isNull())
		settings.addSubitem(StringDataItem("type", QT_TR_NOOP("Type"), proxy->name()));
    
    if (!m_account) {
		// User changed global proxy settings.
		foreach (NetworkProxyManager *manager, NetworkProxyManager::allManagers()) {
			if (proxy && !manager->proxies().contains(proxy))
				continue;
			foreach (Account *account, manager->protocol()->accounts()) {
				if (account->config("proxy").value("useGlobalProxy", true))
					manager->setProxy(account, proxy, settings);
			}
		}
	} else {
		QNetworkProxy::setApplicationProxy(NetworkProxyManager::toNetworkProxy(settings));
		// User changed proxy settings for the account.
		NetworkProxyManager *manager = NetworkProxyManager::get(m_account->protocol());
		if (!manager)
			return;
		if (useGlobal) { // The user chose the global proxy
			cfg.setValue("useGlobalProxy", true);
			proxy = NetworkProxyInfo::proxy(globalCfg.value("type", QString()));
			if (proxy) {
				settings = proxy->settings(globalCfg);
				if (settings.subitem("type").isNull())
					settings.addSubitem(StringDataItem("type", QT_TR_NOOP("Type"), proxy->name()));
				manager->setProxy(m_account, proxy, settings);
			}
		} else {
			cfg.setValue("useGlobalProxy", false);
			manager->setProxy(m_account, proxy, settings);
		}
	}
    cfg.sync();
    globalCfg.sync();
}

QVariantMap QuickProxyHelper::load()
{
    Config cfg = m_account ? m_account->config() : Config();
	cfg = cfg.group("proxy");
    
    QString currentName	= cfg.value("type", QString());
	bool disabled = cfg.value("disabled", m_account ? false : true);
	bool useGlobal = m_account ? cfg.value("useGlobalProxy", true) : false;
    
    QVariantMap result;
    result.insert("currentName", currentName);
    result.insert("disabled", disabled);
    result.insert("useGlobal", useGlobal);
    return result;
}

} // namespace MeegoIntegration
