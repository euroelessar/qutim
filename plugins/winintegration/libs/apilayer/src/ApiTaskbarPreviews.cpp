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

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0601
#endif
#include "ApiTaskbarPreviews.h"
#include "ApiTaskbarPreviewsWAttributes.h"
#include <qt_windows.h>
#include <shobjidl.h>
#include <QLibrary>

enum qutim_DWMWINDOWATTRIBUTE
{
    qutim_DWMWA_NCRENDERING_ENABLED = 1,
    qutim_DWMWA_NCRENDERING_POLICY,
    qutim_DWMWA_TRANSITIONS_FORCEDISABLED,
    qutim_DWMWA_ALLOW_NCPAINT,
    qutim_DWMWA_CAPTION_BUTTON_BOUNDS,
    qutim_DWMWA_NONCLIENT_RTL_LAYOUT,
    qutim_DWMWA_FORCE_ICONIC_REPRESENTATION,
    qutim_DWMWA_FLIP3D_POLICY,
    qutim_DWMWA_EXTENDED_FRAME_BOUNDS,
    qutim_DWMWA_HAS_ICONIC_BITMAP,
    qutim_DWMWA_DISALLOW_PEEK,
    qutim_DWMWA_EXCLUDED_FROM_PEEK,
    qutim_DWMWA_CLOAK,
    qutim_DWMWA_CLOAKED,
    qutim_DWMWA_FREEZE_REPRESENTATION,
    qutim_DWMWA_LAST
};

typedef HRESULT (STDAPICALLTYPE *DwmSetWindowAttribute_t)(HWND, DWORD, LPCVOID, DWORD);
static DwmSetWindowAttribute_t qutim_DwmSetWindowAttribute = 0;

typedef HRESULT (STDAPICALLTYPE *DwmSetIconicThumbnail_t)(HWND, HBITMAP, DWORD);
static DwmSetIconicThumbnail_t qutim_DwmSetIconicThumbnail = 0;

typedef HRESULT (STDAPICALLTYPE *DwmSetIconicLivePreviewBitmap_t)(HWND, HBITMAP, POINT*, DWORD);
static DwmSetIconicLivePreviewBitmap_t qutim_DwmSetIconicLivePreviewBitmap = 0;

typedef HRESULT (STDAPICALLTYPE *DwmInvalidateIconicBitmaps_t)(HWND);
static DwmInvalidateIconicBitmaps_t qutim_DwmInvalidateIconicBitmaps = 0;

static void qutim_init_dwm()
{
	static bool inited = false;
	if (inited)
		return;
	inited = true;

	QLibrary dwm("dwmapi");
	qutim_DwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(dwm.resolve("DwmSetWindowAttribute"));
	qutim_DwmSetIconicThumbnail = reinterpret_cast<DwmSetIconicThumbnail_t>(dwm.resolve("DwmSetIconicThumbnail"));
	qutim_DwmSetIconicLivePreviewBitmap = reinterpret_cast<DwmSetIconicLivePreviewBitmap_t>(dwm.resolve("DwmSetIconicLivePreviewBitmap"));
	qutim_DwmInvalidateIconicBitmaps = reinterpret_cast<DwmInvalidateIconicBitmaps_t>(dwm.resolve("DwmInvalidateIconicBitmaps"));
}

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
	qutim_init_dwm();
	if (qutim_DwmSetIconicThumbnail)
		qutim_DwmSetIconicThumbnail(tab, bitmap, 0);
}

void SetTabLivePreview(HWND tab, HBITMAP bitmap)
{
	qutim_init_dwm();
	if (qutim_DwmSetIconicLivePreviewBitmap)
		qutim_DwmSetIconicLivePreviewBitmap(tab, bitmap, 0, 0);
}

void ForceIconicRepresentation(HWND tab)
{
	qutim_init_dwm();
	int param = TRUE;
	if (qutim_DwmSetWindowAttribute && qutim_DwmSetIconicThumbnail) {
		qutim_DwmSetWindowAttribute(tab, qutim_DWMWA_FORCE_ICONIC_REPRESENTATION, &param, sizeof(param));
		qutim_DwmSetWindowAttribute(tab, qutim_DWMWA_HAS_ICONIC_BITMAP,           &param, sizeof(param));
	}
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
	qutim_init_dwm();
	if (qutim_DwmInvalidateIconicBitmaps)
		qutim_DwmInvalidateIconicBitmaps(hwnd);
}

