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

#include <ShObjIdl.h>
#include "ApiOverlayIcon.h"

void SetOverlayIcon(HWND hwnd, HICON hicon, wchar_t *descr)
{
	if(!hwnd)
		return;
	ITaskbarList3 *taskbar;
	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&taskbar))
		return;
	taskbar->HrInit();
	if(descr)
		taskbar->SetOverlayIcon(hwnd, hicon, descr);
	else
		taskbar->SetOverlayIcon(hwnd, hicon, L"No description");
	taskbar->Release();
}

void ClearOverlayIcon(HWND hwnd)
{
	SetOverlayIcon(hwnd, 0, 0);
}

