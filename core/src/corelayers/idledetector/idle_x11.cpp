/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Roman Tretyakov <roman@trett.ru>
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
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)

#include <QApplication>
#include <QDesktopWidget>
#include <xcb/xcb.h>
#include <xcb/screensaver.h>
#include <QtX11Extras/QX11Info>

namespace Psi {
static xcb_screen_t * screen;

IdlePlatform::IdlePlatform()
{
	screen = xcb_setup_roots_iterator (xcb_get_setup (QX11Info::connection())).data;
}

IdlePlatform::~IdlePlatform()
{

}

bool IdlePlatform::init()
{
	return true;
}

int IdlePlatform::secondsIdle()
{
	xcb_screensaver_query_info_cookie_t cookie = xcb_screensaver_query_info (QX11Info::connection(), screen->root);
	xcb_screensaver_query_info_reply_t *info = xcb_screensaver_query_info_reply (QX11Info::connection(), cookie, NULL);

	uint idle = info->ms_since_user_input;
	free (info);

	return idle/1000;
}

} // namespace Psi

#endif



