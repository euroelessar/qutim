/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
		case Authorization:
			return Status::Connecting; //TODO
		default:
			return Status::Offline;
	}
	
}

QDir getAvatarsDir()
{
	return SystemInfo::getDir(SystemInfo::ConfigDir).filePath(QLatin1String("avatars/vkontakte"));
}

