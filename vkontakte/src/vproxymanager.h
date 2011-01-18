/****************************************************************************
 *  vproxymanager.h
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
 ***************************************************************************/

#ifndef VPROXYMANAGER_H
#define VPROXYMANAGER_H

#include "vkontakte_global.h"
#include <qutim/networkproxy.h>

class VProxyManager : public QObject, public NetworkProxyManager
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "VkontakteProtocol")
	Q_INTERFACES(qutim_sdk_0_3::NetworkProxyManager)
public:
	VProxyManager();
	QList<NetworkProxyInfo*> proxies();
	void setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings);
};

#endif // VPROXYMANAGER_H
