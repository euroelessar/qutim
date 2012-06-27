/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "proxysettings.h"
#include <qutim/settingslayer.h>
#include <qutim/networkproxy.h>
#include <qutim/dataforms.h>
#include <qutim/icon.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <QFormLayout>
#include <QStackedLayout>
#include <QComboBox>

namespace Core {

ProxySettingsWidget::ProxySettingsWidget() :
	m_settingsLayout(0), m_account(0)
{
	m_layout = new QFormLayout(this);
	m_typeBox = new QComboBox(this);
	m_typeBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_layout->addRow(tr("Type:"), m_typeBox);
	connect(m_typeBox, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged(int)));
	lookForWidgetState(m_typeBox);
}

void ProxySettingsWidget::typeChanged(int index)
{
	if (m_account && index >= 1)
		--index;
	m_settingsLayout->setCurrentIndex(index);
}

void ProxySettingsWidget::loadImpl()
{
	// Clear old data
	if (m_settingsLayout)
		delete m_settingsLayout;
	m_settingsLayout = new QStackedLayout();
	m_layout->addRow(m_settingsLayout);
	m_typeBox->clear();

	Config cfg = m_account ? m_account->config() : Config();
	cfg = cfg.group("proxy");
	QString currentName	= cfg.value("type", QString());
	int currentIndex = -1, i = 0;
	bool disabled = cfg.value("disabled", m_account ? false : true);
	bool useGlobal = m_account ? cfg.value("useGlobalProxy", true) : false;

	// Initialize widget
	m_typeBox->addItem(tr("Do not use proxy"));
	m_settingsLayout->addWidget(new QWidget());
	if (m_account)
		m_typeBox->addItem(tr("Use global proxy"));
	foreach (NetworkProxyInfo *proxy, proxies()) {
		m_typeBox->addItem(proxy->icon(), proxy->description());
		QWidget *w = AbstractDataForm::get(proxy->settings(cfg));
		if (!w)
			w = new QWidget(this);
		m_settingsLayout->addWidget(w);
		if (!disabled && !useGlobal) {
			if (currentIndex == -1 && proxy->name() == currentName)
				currentIndex = i;
			++i;
		}
	}
	// Set current page
	if (!disabled)
		currentIndex += m_account ? 2 : 1;
	else
		currentIndex = 0;
	m_typeBox->setCurrentIndex(currentIndex);
	listenChildrenStates(QWidgetList() << m_typeBox); // m_typeBox was added in constructor
}

void ProxySettingsWidget::saveImpl()
{
	NetworkProxyInfo *proxy = proxies().value(m_settingsLayout->currentIndex()-1);
	AbstractDataForm *w = qobject_cast<AbstractDataForm*>(m_settingsLayout->currentWidget());
	DataItem settings = w ? w->item() : DataItem();
	Config globalCfg = Config().group("proxy");
	Config cfg = m_account ? m_account->config("proxy") : globalCfg;

	if (m_typeBox->currentIndex() == 0) {
		// User disabled proxy
		cfg.setValue("disabled", true);
		if (m_account)
			cfg.setValue("useGlobalProxy", false);
	} else {
		cfg.setValue("disabled", false);
		if (proxy)
			proxy->saveSettings(cfg, settings); // Save settings to the config.
	}

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
		if (m_typeBox->currentIndex() == 1) { // The user chose the global proxy
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
}

void ProxySettingsWidget::cancelImpl()
{
	loadImpl();
}

void ProxySettingsWidget::setController(QObject *controller)
{
	m_account = qobject_cast<Account*>(controller);
}


QList<NetworkProxyInfo*> ProxySettingsWidget::proxies()
{
	if (m_account) {
		NetworkProxyManager *manager = NetworkProxyManager::get(m_account->protocol());
		return manager ? manager->proxies() : QList<NetworkProxyInfo*>();
	} else {
		return NetworkProxyInfo::allProxies();
	}
}

ProxySettings::ProxySettings()
{
	GeneralSettingsItem<ProxySettingsWidget> *item =
			new GeneralSettingsItem<ProxySettingsWidget>(
					Settings::Protocol,
					Icon("preferences-system-network-proxy"),
					QT_TRANSLATE_NOOP("Settings","Global proxy")
					);
	Settings::registerItem(item);

	item = new GeneralSettingsItem<ProxySettingsWidget>(
					Settings::Protocol,
					Icon("preferences-system-network-proxy"),
					QT_TRANSLATE_NOOP("Settings","Proxy")
					);
	Settings::registerItem<Account>(item);
	deleteLater();

}

} // namespace Core

