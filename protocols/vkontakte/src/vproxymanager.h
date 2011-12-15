/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

