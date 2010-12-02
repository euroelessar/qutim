/****************************************************************************
 *  networkproxy.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef NETWORKPROXY_H
#define NETWORKPROXY_H

#include "libqutim_global.h"
#include "config.h"
#include <QNetworkProxy>

class QIcon;

namespace qutim_sdk_0_3
{
	class LocalizedString;
	class DataItem;
	class Protocol;
	class Account;
	class NetworkProxyManagerPrivate;

	/**
	  The NetworkProxyInfo class describes a proxy type.
	  */
	class LIBQUTIM_EXPORT NetworkProxyInfo
	{
	public:
		NetworkProxyInfo();
		virtual ~NetworkProxyInfo();
		/**
		 Returns the \b unique proxy name.

		 When implementing a NetworkProxyInfo in a plugin, do not forget
		 that the proxy name is unique. To avoid conflicts with other plugins
		 always add prefix to the name. For instance: "jabber/http".
		 */
		virtual QString name() = 0;
		/**
		 Returns the icon of the proxy.
		 */
		virtual QIcon icon();
		/**
		 Returns the proxy description.
		 */
		virtual LocalizedString description() = 0;
		/**
		 Returns the settings data form for the proxy.

		 You may want to consider to use NetworkProxyManager::settings(Account*)
		 instead of this method.

		 \see NetworkProxyManager::settings(), NetworkProxyManager::settings(Account*)
		 */
		virtual DataItem settings(const Config &config) = 0;
		/**
		  Saves the \a setting to the \a config.
		  */
		virtual void saveSettings(Config config, const DataItem &settings) = 0;
		/**
		  Returns the NetworkProxyInfo object by the \a name.
		  */
		static NetworkProxyInfo *proxy(const QString &name);
		/**
		  Return all supported proxies.
		  */
		static QList<NetworkProxyInfo*> allProxies();
	protected:
		virtual void virtual_hook(int id, void *data);
	};

	/**
	  The NetworkProxyManager class provides an interface to manipulate proxies.

	  The class is used to get the list of proxies and to set a new proxy to be used
	  by an account.
	  */
	class LIBQUTIM_EXPORT NetworkProxyManager
	{
	public:
		/**
		  Constructs a new NetworkProxyManager with the given \a protocol.
		  */
		NetworkProxyManager(Protocol *protocol);
		/**
		  Destroyes the object.
		  */
		virtual ~NetworkProxyManager();
		/**
		  Returns the protocol this NetworkProxyManager is for.
		  */
		Protocol *protocol();
		/**
		  Returns the list of proxies.
		  */
		virtual QList<NetworkProxyInfo*> proxies() = 0;
		/**
		  Sets the new \a proxy to be used by the \a account.

		  \param settings the settings data form this proxy will be initialized with.
		  */
		virtual void setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings) = 0;
		/**
		  Returns the NetworkProxyManager for the \a protocol.
		  */
		static NetworkProxyManager *get(Protocol *protocol);
		/**
		  Returns all proxy managers.
		  */
		static QList<NetworkProxyManager*> allManagers();
		/**
		  Returns global proxy settings.
		  */
		static DataItem settings();
		/**
		  Returns proxy settings for the \a account.

		  If the \a account uses global proxy settings, global
		  proxy settings will be returned.
		  \see settings().
		  */
		static DataItem settings(Account *account);
		/**
		  Converts the proxy \a settings to QNetworkProxy.
		  */
		static QNetworkProxy toNetworkProxy(const DataItem &settings);
	protected:
		virtual void virtual_hook(int id, void *data);
	private:
		NetworkProxyManagerPrivate *p;
	};

	/**
	  Default implementation of sock5 proxy.
	  */
	class LIBQUTIM_EXPORT Socks5ProxyInfo : public NetworkProxyInfo
	{
	public:
		virtual QString name();
		virtual LocalizedString description();
		virtual DataItem settings(const Config &config);
		virtual void saveSettings(Config config, const DataItem &settings);
		/**
		  Returns a pointer to the sock5 proxy object.
		  */
		static Socks5ProxyInfo *instance();
	protected:
		Socks5ProxyInfo();
	};

	/**
	  Default implementation of http proxy.
	  */
	class LIBQUTIM_EXPORT HttpProxyInfo : public NetworkProxyInfo
	{
	public:
		virtual QString name();
		virtual LocalizedString description();
		virtual DataItem settings(const Config &config);
		virtual void saveSettings(Config config, const DataItem &settings);
		/**
		  Returns a pointer to the http proxy object.
		  */
		static HttpProxyInfo *instance();
	protected:
		HttpProxyInfo();
	};

} // namespace qutim_sdk_0_3

Q_DECLARE_INTERFACE(qutim_sdk_0_3::NetworkProxyManager, "org.qutim.core.NetworkProxyManager");

#endif // NETWORKPROXY_H
