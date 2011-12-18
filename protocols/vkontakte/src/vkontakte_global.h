/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef VKONTAKTE_GLOBAL_H
#define VKONTAKTE_GLOBAL_H

#if defined(VKONTAKTE_LIBRARY)
#  define LIBVKONTAKTE_EXPORT Q_DECL_EXPORT
#else
#  define LIBVKONTAKTE_EXPORT Q_DECL_IMPORT
#endif

#include <qutim/libqutim_global.h>
#include <qutim/status.h>

using namespace qutim_sdk_0_3;

class QDir;

enum VConnectionState {
	Invalid = -1,
	Connected,
	Connecting,
	Disconnected,
	Authorization
};

VConnectionState LIBVKONTAKTE_EXPORT statusToState(Status::Type);
Status::Type LIBVKONTAKTE_EXPORT stateToStatus(VConnectionState state);
QDir LIBVKONTAKTE_EXPORT getAvatarsDir();

#endif // VKONTAKTE_GLOBAL_H

