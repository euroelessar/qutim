/*
 * idle_x11.cpp - detect desktop idle time
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "idle.h"

#ifdef Q_WS_MAEMO_5
#include <qapplication.h>
#include <QtDBus>

#include <mce/mode-names.h>
#include <mce/dbus-names.h>

namespace Psi
{
class IdlePlatform::Private:public QObject
{
public:
	Private() {}
private slots:
	void setDisplayState(const QString &state)
	{
		if (!state.isEmpty()) {
			if (state == MCE_DISPLAY_ON_STRING)
			{
				display_off=false;
				idle_timer->stop();
			}
			else if (state == MCE_DISPLAY_OFF_STRING)
			{
				display_off=true;
				idle_timer->start();
			}
		}
	}
	void displayStateChanged(const QDBusMessage &message)
	{
		QString state = message.arguments().at(0).toString();
		setDisplayState(state);
	}

private:
	QDBusInterface *mDbusInterface;
	bool display_off;
	QTimer *idle_timer;

public:
	void Init()
	{
		mDbusInterface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH,
											MCE_REQUEST_IF, QDBusConnection::systemBus(),
											this);
		display_off=false;
		QDBusConnection::systemBus().connect(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
											 MCE_DISPLAY_SIG, this,SLOT(displayStateChanged(const QDBusMessage &)));
		mDbusInterface->callWithCallback(MCE_DISPLAY_STATUS_GET, QList<QVariant>(), this, SLOT(setDisplayState(const QString &)));
		idle_timer = new QTimer();
	}
	int GetIdleSeconds()
	{
		return idle_timer->interval()/1000;
	}

};

IdlePlatform::IdlePlatform()
{
	d = new Private;
}

IdlePlatform::~IdlePlatform()
{
	d->deleteLater();
}

bool IdlePlatform::init()
{
	d->Init();
	return true;
}

int IdlePlatform::secondsIdle()
{
	return d->GetIdleSeconds();
}
}
#endif
