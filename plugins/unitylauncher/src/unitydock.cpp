/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "unitydock.h"
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
	if (m_menu)
		delete m_menu.data();
	if (menu) {
		m_menu = new DBusMenuExporter("/qutim", menu);
		sendMessage("quicklist", "/qutim");
	} else {
		sendMessage("quicklist", "");
	}
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

