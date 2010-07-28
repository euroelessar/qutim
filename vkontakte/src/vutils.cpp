/****************************************************************************
 *  vutils.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#include "vkontakte_global.h"
#include "vkontakteprotocol.h"
#include <qutim/systeminfo.h>

VConnectionState statusToState(Status::Type status)
{
	VConnectionState state;
	switch (status) {
		case Status::Offline:
			state = Disconnected;
			break;
		case Status::Connecting:
			state = Connecting;
			break;
		default:
			state = Connected;
	};
	return state;
}

Status::Type stateToStatus(VConnectionState state)
{
	switch (state) {
		case Disconnected:
			return Status::Offline;
		case Connected:
			return Status::Online;
		case Connecting:
			return Status::Connecting;
		default:
			return Status::Offline;
	}
	
}

QDir getAvatarsDir()
{
	return SystemInfo::getDir(SystemInfo::ConfigDir).filePath(QLatin1String("avatars/vkontakte"));
}
