#include "ApiTaskbarPreviews.h"
#include "Shobjidl.h"
#include <dwmapi.h>

#include <comdef.h>
#include <iostream>
using namespace std;

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
	int Param = TRUE;
	int e1 = DwmSetWindowAttribute(tab, DWMWA_FORCE_ICONIC_REPRESENTATION, &Param, sizeof(Param));
	int e2 = DwmSetWindowAttribute(tab, DWMWA_HAS_ICONIC_BITMAP, &Param, sizeof(Param));
	/*_com_error er1(e1), er2(e2);
	cerr << "\nForceIconicRepresentation " << tab << endl;
	cerr << er1.ErrorMessage() << " " << er2.ErrorMessage() << endl;*/
}

void UnregisterTab(HWND tab)
{
	ITaskbarList4 *taskbar;

	if(!(tab))
		return;

	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList4, (void**)&taskbar))
		return;
	taskbar->HrInit();
	taskbar->UnregisterTab(tab);
	taskbar->Release();
}

void InvalidateBitmaps(HWND hwnd)
{
	DwmInvalidateIconicBitmaps(hwnd);
}
