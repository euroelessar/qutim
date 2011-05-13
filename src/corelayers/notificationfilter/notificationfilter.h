/****************************************************************************
 *  notificationfilter.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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


#ifndef NOTIFICATIONFILTER_H
#define NOTIFICATIONFILTER_H

#include <qutim/notification.h>
#include <qutim/startupmodule.h>

namespace Core {

class NotificationFilterImpl : public QObject, public qutim_sdk_0_3::NotificationFilter, public qutim_sdk_0_3::StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::NotificationFilter qutim_sdk_0_3::StartupModule)
public:
	NotificationFilterImpl();
	virtual ~NotificationFilterImpl();
	virtual Result filter(qutim_sdk_0_3::NotificationRequest &request);
private slots:
	void onOpenChatClicked(const qutim_sdk_0_3::NotificationRequest &request);
	void onIgnoreChatClicked(const qutim_sdk_0_3::NotificationRequest &request);
};

} // namespace Core

#endif // NOTIFICATIONFILTER_H
