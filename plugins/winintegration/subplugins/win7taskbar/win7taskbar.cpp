/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ivan Vizir <define-true-false@yandex.com>
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

#include "win7taskbar.h"
#include "thumbnails.h"
#include "overlay-icon.h"
#include "../../src/winint.h"
#include <QSysInfo>
#include <qutim/chatsession.h>
#include <qutim/servicemanager.h>
#include <WinThings/TaskbarPreviews.h>
#include <QtWin>

using namespace qutim_sdk_0_3;

void Win7Features::init()
{
	thumbnails  = 0;
	overlayIcon = 0;
	setCapabilities(Loadable);
}

bool Win7Features::load()
{
	if (QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7)
		return false;
	thumbnails  = new WThumbnails;
	overlayIcon = new WOverlayIcon;
	//connect(WinIntegration::instance(), SIGNAL(reloadSettigs()), thumbnails,  SLOT(reloadSetting())); // no need - loads them once
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), thumbnails, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	if (QWidget *widget = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>()) {
		QWindow *window = widget->windowHandle();
		QtWin::setWindowFlip3DPolicy(window, QtWin::FlipExcludeBelow);
		QtWin::setWindowExcludedFromPeek(window, true);
	}
	WinIntegration::instance()->enabledPlugin(WI_Win7Taskbar);
	return true;
}

bool Win7Features::unload()
{
	if (QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7)
		return false;
	delete thumbnails;
	delete overlayIcon;
	thumbnails  = 0;
	overlayIcon = 0;
	if (QWidget *widget = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>()) {
		QWindow *window = widget->windowHandle();
		QtWin::setWindowFlip3DPolicy(window, QtWin::FlipDefault);
		QtWin::setWindowExcludedFromPeek(window, false);
	}
	WinIntegration::instance()->disabledPlugin(WI_Win7Taskbar);
	return true;
}


