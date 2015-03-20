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

#include <shobjidl.h>
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

