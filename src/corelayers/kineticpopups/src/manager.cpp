/****************************************************************************
*  manager.cpp
*
*  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "manager.h"
#include "popup.h"
#include <QDebug>
#include <QFile>
#include <QTextDocument>
#include <QApplication>
#include <QMouseEvent>
#include <QDesktopWidget>
#include "qutim/systeminfo.h"
#include "qutim/configbase.h"

namespace Core
{
namespace KineticPopups
{
// the universe's only Notification manager
Manager *Manager::instance = 0;

Manager::Manager()
{
	loadSettings();
}

Popup *Manager::getById ( const QString& id ) const
{
	int number = getNumber(id);
	return (number == -1) ? 0 : active_notifications.value(number);
}


int Manager::getNumber ( const QString& id ) const
{
	for (int i=0;i!=active_notifications.count();i++) {
		if (active_notifications.value(i)->getId() == id)
			return i;
	}
	return -1;
}


Popup* Manager::getByNumber ( const int& number ) const
{
	return active_notifications.value(number);
}

int Manager::count()
{
	return active_notifications.count();
}

QRect Manager::insert ( Popup* notification )
{
	if (getNumber(notification->getId()) != -1)
	{
		qWarning() << "Trying to overwrite exist notification";
		return QRect();
	}
	active_notifications.append(notification);
	int number = active_notifications.count()-1;
	if (number == 0) {
		QRect geom = QApplication::desktop()->availableGeometry(QCursor::pos());
		geom = QRect(geom.bottomRight(),notification->geometry().size());
		geom.moveLeft(geom.x() - margin - notification->geometry().width());
		return geom;
	}
	else
		return active_notifications.at(number-1)->geometry();
}


void Manager::remove ( const QString& id )
{
	int number = getNumber(id);
	active_notifications.removeAt(number);
	updateGeometry();
}


void Manager::updateGeometry()
{
	if (updatePosition) {
		QRect geom = QApplication::desktop()->availableGeometry(QCursor::pos());
		int y = geom.bottom();
		for (int i=0;i!=active_notifications.count();i++) {
			y -= margin + active_notifications.value(i)->geometry().height();
			geom.moveTop(y);
			QRect widget_geom = geom;
			widget_geom.moveLeft(geom.right() - margin - active_notifications.value(i)->geometry().width());
			widget_geom.setSize(active_notifications.value(i)->geometry().size());
			active_notifications.value(i)->update(widget_geom);
		}
	}
}




Manager* Manager::self()
{
	if ( !instance )
		instance = new Manager();
	return instance;
}


void Manager::loadSettings()
{
	Config behavior = Config("behavior").group("notifications/popups");
	themeName = behavior.value<QString>("themeName","default");
	updatePosition = behavior.value<bool>("updatePosition",true);
	animation = static_cast<AnimationFlags>(behavior.value<int>("animationFlags",Opacity));
	timeout = behavior.value<int>("timeout",5000);
	easingCurve.setType(static_cast<QEasingCurve::Type>(behavior.value<int>("easingCurve",QEasingCurve::OutSine)));
	maxCount = behavior.value<int>("maxCount",10);
	maxTextLength = behavior.value<int>("maxTextLength",160);
	appendMode = behavior.value<bool>("appendMode",true);
	updateMode = behavior.value<bool>("updateMode",false);
	animationDuration = behavior.value("animationDuration",600);
	showFlags = static_cast<NotificationTypes>(behavior.value("showFlags", 0xfffffff &~ Notifications::MessageSend));
	margin = behavior.value("margin",20);

	Config general = Config("appearance").group("qmlpopups/general");
	themeName = general.value<QString>("themeName","default");
}

}
}
