#include "ApiTaskbarPreviews.h"
#include "ApiTaskbarPreviewsWAttributes.h"
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
