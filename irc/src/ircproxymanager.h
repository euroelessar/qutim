/****************************************************************************
 *  ircproxymanager.h
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

#ifndef IRCPROXYMANAGER_H
#define IRCPROXYMANAGER_H

#include "ircglobal.h"
#include <qutim/networkproxy.h>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcProxyManager : public QObject, public NetworkProxyManager
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::irc::IrcProtocol")
	Q_INTERFACES(qutim_sdk_0_3::NetworkProxyManager)
public:
	IrcProxyManager();
	QList<NetworkProxyInfo*> proxies();
	void setProxy(Account *account, NetworkProxyInfo *proxy, const DataItem &settings);
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCPROXYMANAGER_H
