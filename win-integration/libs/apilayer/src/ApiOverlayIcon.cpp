#include <ShObjIdl.h>
#include <atlbase.h>
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
