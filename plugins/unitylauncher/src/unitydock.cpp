/****************************************************************************
 * unitydock.cpp
 *  Copyright © 2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv2 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include "unitydock.h"
#include <dbusmenuexporter.h>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QVariantList>
#include <QVariantMap>

UnityDock::UnityDock(QObject *p) :
	DockBase(p)
{
}

UnityDock::~UnityDock()
{
}

template<typename T> void UnityDock::sendMessage(const char *name, const T& val)
{
	QDBusMessage message = QDBusMessage::createSignal("/qutim", "com.canonical.Unity.LauncherEntry", "Update");
	QVariantList args;
	QVariantMap map;
	map.insert(QLatin1String(name), val);
	args << QLatin1String("application://qutim.desktop") << map;
	message.setArguments(args);
	QDBusConnection::sessionBus().send(message);
}

void UnityDock::setIcon(const QIcon &)
{
}

void UnityDock::setOverlayIcon(const QIcon &)
{
}

void UnityDock::setMenu(QMenu *menu)
{
	if(menu)
	{
		DBusMenuExporter exporter("/qutim", menu);
		sendMessage("quicklist", "/qutim");
	}
	else
		sendMessage("quicklist", "");
}

void UnityDock::setProgress(int progress)
{
	progress = qBound(0, progress, 100);
	sendMessage("progress", static_cast<double>(progress)/100.0);
	sendMessage("progress-visible", progress > 0);
}

void UnityDock::setBadge(const QString &badge)
{
	setCount(badge.toInt());
}

void UnityDock::setCount(int count)
{
	sendMessage("count", static_cast<qint64>(count));
	sendMessage("count-visible", count > 0);
}


void UnityDock::setAlert(bool on)
{
	sendMessage("urgent", on);
}
