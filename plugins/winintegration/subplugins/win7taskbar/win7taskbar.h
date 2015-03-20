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

#ifndef WIN7TASKBAR_H
#define WIN7TASKBAR_H

#include <qutim/plugin.h>

class WThumbnails;
class WOverlayIcon;

class Win7Features : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "WinIntegration/Win7Features")
	Q_CLASSINFO("DependsOn", "WinIntegration")
	//Q_CLASSINFO("Service",   "Dock")

public:
	void init();
	bool load();
	bool unload();

private:
	WThumbnails  *thumbnails;
	WOverlayIcon *overlayIcon;
};

#endif // WIN7TASKBAR_H
