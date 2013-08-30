/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2003 Justin Karneges <justin@affinix.com>
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

#include "idle.h"
#include <QDebug>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#include <QApplication>
#include <QDesktopWidget>
#include <xcb/xcb.h>
#include <xcb/screensaver.h>

namespace Psi {
static xcb_connection_t * connection;
static xcb_screen_t * screen;

IdlePlatform::IdlePlatform()
{

}

IdlePlatform::~IdlePlatform()
{

}

bool IdlePlatform::init()
{
	connection = xcb_connect (NULL, NULL);
	screen = xcb_setup_roots_iterator (xcb_get_setup (connection)).data;
	return true;
}

int IdlePlatform::secondsIdle()
{
	xcb_screensaver_query_info_cookie_t cookie;
	xcb_screensaver_query_info_reply_t *info;

	cookie = xcb_screensaver_query_info (connection, screen->root);
	info = xcb_screensaver_query_info_reply (connection, cookie, NULL);

	uint32_t idle = info->ms_since_user_input;
	free (info);

	int i = static_cast<int>(idle);
	return i/1000;
}

} // namespace Psi

#endif



