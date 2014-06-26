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

#define _WIN32_WINNT 0x0601
#include "ApiTaskbarPreviews.h"
#include "ApiTaskbarPreviewsWAttributes.h"
#include <qt_windows.h>
#include "Shobjidl.h"
#include <dwmapi.h>

//#include <comdef.h>
//#include <iostream>
//using namespace std;

void RegisterTab(HWND tab, HWND owner)
{
	ITaskbarList4 *taskbar;

	if(!(tab && owner))
		return;

	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList4, (void**)&taskbar))
		return;
	taskbar->HrInit();
	taskbar->RegisterTab(tab, owner);
	taskbar->Release();
}

void SetTabActive(HWND tab, HWND owner)
{
	ITaskbarList3 *taskbar;

	if(!owner)
		return;

	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&taskbar))
		return;

	taskbar->HrInit();
	taskbar->SetTabActive(tab, owner, 0);
	taskbar->Release();
}

void SetNoTabActive(HWND owner)
{
	SetTabActive(0, owner);
}

void SetTabOrder(HWND tab, HWND insertBefore)
{
	ITaskbarList3 *taskbar;

	if(!tab)
		return;

	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&taskbar))
		return;

	taskbar->HrInit();
	taskbar->SetTabOrder(tab, insertBefore);
	taskbar->Release();
}

void SetTabIconicPreview(HWND tab, HBITMAP bitmap)
{
	DwmSetIconicThumbnail(tab, bitmap, 0);
}

void SetTabLivePreview(HWND tab, HBITMAP bitmap)
{
	DwmSetIconicLivePreviewBitmap(tab, bitmap, 0, 0);
}

void ForceIconicRepresentation(HWND tab)
{
	int param = TRUE;
	DwmSetWindowAttribute(tab, DWMWA_FORCE_ICONIC_REPRESENTATION, &param, sizeof(param));
	DwmSetWindowAttribute(tab, DWMWA_HAS_ICONIC_BITMAP,           &param, sizeof(param));
	// _com_error er1(e1), er2(e2);
}

void UnregisterTab(HWND tab)
{
	ITaskbarList3 *taskbar;

	if(!(tab))
		return;

	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&taskbar))
		return;
	taskbar->HrInit();
	taskbar->UnregisterTab(tab);
	taskbar->Release();
}

void InvalidateBitmaps(HWND hwnd)
{
	DwmInvalidateIconicBitmaps(hwnd);
}

void SetWindowAttributes(HWND hwnd, unsigned attr)
{
	BOOL t = TRUE;
	BOOL f = FALSE;
	DWMFLIP3DWINDOWPOLICY policy;
	if (TA_Flip3D_ExcludeAbove & attr)
		policy = DWMFLIP3D_EXCLUDEABOVE;
	else if (TA_Flip3D_ExcludeBelow & attr)
		policy = DWMFLIP3D_EXCLUDEBELOW;
	else
		policy = DWMFLIP3D_DEFAULT;
	DwmSetWindowAttribute(hwnd, DWMWA_FLIP3D_POLICY,      &policy,                          sizeof(policy));
	DwmSetWindowAttribute(hwnd, DWMWA_DISALLOW_PEEK,      TA_Peek_Disallow    & attr ? &t: &f, sizeof(BOOL));
	DwmSetWindowAttribute(hwnd, DWMWA_EXCLUDED_FROM_PEEK, TA_Peek_ExcludeFrom & attr ? &t: &f, sizeof(BOOL));
}

