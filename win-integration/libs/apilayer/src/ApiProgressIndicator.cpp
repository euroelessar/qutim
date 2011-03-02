#include <ShObjIdl.h>
#include "ApiProgressIndicator.h"

void SetProgressValEx(HWND hWnd, ULONGLONG Completed, ULONGLONG Total)
{
	ITaskbarList3 *taskbar;
	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&taskbar))
		return;
	taskbar->HrInit();
	taskbar->SetProgressValue(hWnd, Completed, Total);
	taskbar->Release();
}

void SetProgressVal(HWND hWnd, int Percents)
{
	SetProgressValEx(hWnd, Percents, 100);
}

void ClearProgress(HWND hWnd)
{
	SetProgressState(hWnd, PS_None);
}

void SetProgressState(HWND hWnd, ProgressStates state)
{
	TBPFLAG flags;
	ITaskbarList3 *taskbar;
	if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&taskbar))
		return;
	taskbar->HrInit();
	switch (state)	{
		default:
		case PS_None          : flags = TBPF_NOPROGRESS;    break;
		case PS_Normal        : flags = TBPF_NORMAL;        break;
		case PS_Paused        : flags = TBPF_PAUSED;        break;
		case PS_Error         : flags = TBPF_ERROR;         break;
		case PS_Indeterminate : flags = TBPF_INDETERMINATE; break;
	}
	taskbar->SetProgressState(hWnd, flags);
	taskbar->Release();
}
