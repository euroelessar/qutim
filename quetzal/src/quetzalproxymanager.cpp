///****************************************************************************
// *
// *  This file is part of qutIM
// *
// *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
// *
// ***************************************************************************
// *                                                                         *
// *   This file is part of free software; you can redistribute it and/or    *
// *   modify it under the terms of the GNU General Public License as        *
// *   published by the Free Software Foundation; either version 2 of the    *
// *   License, or (at your option) any later version.                       *
// *                                                                         *
// ***************************************************************************
// ****************************************************************************/

//#include "quetzalproxymanager.h"
//#include "quetzalprotocol.h"
//#include <qutim/metaobjectbuilder.h>
//#include "quetzalaccount.h"

//using namespace qutim_sdk_0_3;

//QuetzalProxyInfo::QuetzalProxyInfo(PurpleProxyType type)
//{
//}

//QuetzalProxyInfo::~QuetzalProxyInfo()
//{
//}

//QString QuetzalProxyInfo::name()
//{
//	switch (m_type) {
//	case PURPLE_PROXY_HTTP:
//		return QLatin1String("quetzal-http");
//	case PURPLE_PROXY_SOCKS4:
//		return QLatin1String("quetzal-socks4");
//	case PURPLE_PROXY_SOCKS5:
//		return QLatin1String("quetzal-socks5");
//	case PURPLE_PROXY_USE_ENVVAR:
//		return QLatin1String("quetzal-envvar");
//	case PURPLE_PROXY_USE_GLOBAL:
//	case PURPLE_PROXY_NONE:
//	default:
//		return QString();
//	}
//}

//LocalizedString QuetzalProxyInfo::description()
//{
//	switch (m_type) {
//	case PURPLE_PROXY_HTTP:
//		return QT_TRANSLATE_NOOP("Proxy", "HTTP");
//	case PURPLE_PROXY_SOCKS4:
//		return QT_TRANSLATE_NOOP("Proxy", "Socks 4");
//	case PURPLE_PROXY_SOCKS5:
//		return QT_TRANSLATE_NOOP("Proxy", "Socks 5");
//	case PURPLE_PROXY_USE_ENVVAR:
//		return QT_TRANSLATE_NOOP("Proxy", "Use environmental settings");
//	case PURPLE_PROXY_USE_GLOBAL:
//	case PURPLE_PROXY_NONE:
//	default:
//		return LocalizedString();
//	}
//}

//DataItem QuetzalProxyInfo::settings(const Config &cfg)
//{
//	DataItem item;
//	if (m_type == PURPLE_PROXY_USE_ENVVAR)
//		return item;
//	item.addSubitem(StringDataItem("host", QT_TRANSLATE_NOOP("Proxy", "Host"),
//								   cfg.value("host", QString())));
//	item.addSubitem(IntDataItem("port", QT_TRANSLATE_NOOP("Proxy", "Port"),
//								cfg.value("port", 3128), 0, 65535));
//	item.addSubitem(StringDataItem("user", QT_TRANSLATE_NOOP("Proxy", "User name"),
//								   cfg.value("user", QString())));
//	item.addSubitem(StringDataItem("password", QT_TRANSLATE_NOOP("Proxy", "Password"),
//								   cfg.value("password", QString(), Config::Crypted), 0, true));
//	return item;
//}

//void QuetzalProxyInfo::saveSettings(Config cfg, const DataItem &settings)
//{
//	cfg.setValue("type", name());
//	if (m_type == PURPLE_PROXY_USE_ENVVAR)
//		return;
//	cfg.setValue("host", settings.subitem("host").data());
//	cfg.setValue("port", settings.subitem("port").data());
//	cfg.setValue("user", settings.subitem("user").data());
//	cfg.setValue("password", settings.subitem("password").data(), Config::Crypted);
//}

//QuetzalProxyManager::QuetzalProxyManager(QuetzalProtocol *protocol)
//	: NetworkProxyManager(protocol)
//{
//}

//QuetzalProxyManagerGenerator::QuetzalProxyManagerGenerator(QuetzalProtocolGenerator *protocol)
//	: m_protocol(protocol)
//{
//	QByteArray name = "Quetzal::ProxyManager::";
//	name += protocol->plugin()->info->id;
//	MetaObjectBuilder builder(name, &QuetzalProxyManager::staticMetaObject);
//	builder.addClassInfo("DependsOn", protocol->metaObject()->className());
//	m_meta = builder.generate();
//}

//QuetzalProxyManager::~QuetzalProxyManager()
//{
//}

//void quetzal_network_proxy_infos_init(QList<NetworkProxyInfo*> *infos)
//{
//	infos->append(new QuetzalProxyInfo(PURPLE_PROXY_HTTP));
//	infos->append(new QuetzalProxyInfo(PURPLE_PROXY_SOCKS4));
//	infos->append(new QuetzalProxyInfo(PURPLE_PROXY_SOCKS5));
//	infos->append(new QuetzalProxyInfo(PURPLE_PROXY_USE_ENVVAR));
//}

//Q_GLOBAL_STATIC_WITH_INITIALIZER(QList<NetworkProxyInfo*>, quetzal_network_proxy_infos_get,
//								 quetzal_network_proxy_infos_init(x.data()))

//QList<qutim_sdk_0_3::NetworkProxyInfo*> QuetzalProxyManager::proxies()
//{
//	return *quetzal_network_proxy_infos_get();
//}

//void QuetzalProxyManager::setProxy(qutim_sdk_0_3::Account *acc,
//											qutim_sdk_0_3::NetworkProxyInfo *proxy,
//											const qutim_sdk_0_3::DataItem &settings)
//{
//	QuetzalAccount *account = qobject_cast<QuetzalAccount*>(acc);
//	if (!account)
//		return;
//	// TODO: Handle no proxy
//	if (!proxy->name().startsWith(QLatin1String("quetzal")))
//		return;
//	QuetzalProxyInfo *info = static_cast<QuetzalProxyInfo*>(proxy);
//	PurpleProxyInfo *purpleInfo = purple_proxy_info_new();
////	purple_proxy_info_set_type(info->type());
////	purple_proxy_info_set_host(purpleInfo, cfg.value("host", QString()).toUtf8().constData());
////	purple_proxy_info_set_port(cfg.value("port", 3128));
////	purple_proxy_info_set_username(cfg.value("user", QString()).toUtf8().constData());
////	purple_proxy_info_set_password(cfg.value("password", QString(), Config::Crypted).toUtf8().constData());
////	account
//}

//QuetzalProxyManagerGenerator::~QuetzalProxyManagerGenerator()
//{
//}

//QObject *QuetzalProxyManagerGenerator::generateHelper() const
//{
//	void *uiInfo = m_protocol->plugin()->info->ui_info;
//	QuetzalProtocol *protocol = reinterpret_cast<QuetzalProtocol*>(uiInfo);
//	return new QuetzalProxyManager(protocol);
//}
