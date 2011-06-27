/****************************************************************************
 *  notificationslayer_p.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef NOTIFICATIONSLAYER_P_H
#define NOTIFICATIONSLAYER_P_H
#include "notification.h"

namespace qutim_sdk_0_3 {

class SoundHandler : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::NotificationBackend)
public:
	SoundHandler();
	virtual void handleNotification(Notification *notification);
};

} //namespace qutim_sdk_0_3

#endif // NOTIFICATIONSLAYER_P_H
