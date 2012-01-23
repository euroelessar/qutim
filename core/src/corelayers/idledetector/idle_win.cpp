/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2003 Justin Karneges <justin@affinix.com>
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

#include "idle.h"

#ifdef Q_OS_WIN32

#include <QLibrary>
#include <windows.h>

#ifndef tagLASTINPUTINFO
typedef struct __tagLASTINPUTINFO {
	UINT cbSize;
	DWORD dwTime;
 } __LASTINPUTINFO, *__PLASTINPUTINFO;
#endif

namespace Psi
{
class IdlePlatform::Private
{
public:
	Private()
	{
		GetLastInputInfo = NULL;
		lib = 0;
	}

	BOOL (__stdcall * GetLastInputInfo)(__PLASTINPUTINFO);
	DWORD (__stdcall * IdleUIGetLastInputTime)(void);
	QLibrary *lib;
};

IdlePlatform::IdlePlatform()
{
	d = new Private;
}

IdlePlatform::~IdlePlatform()
{
	delete d->lib;
	delete d;
}

bool IdlePlatform::init()
{
	if(d->lib)
		return true;
	void *p;

	// try to find the built-in Windows 2000 function
	d->lib = new QLibrary("user32");
	if(d->lib->load() && (p = d->lib->resolve("GetLastInputInfo"))) {
		d->GetLastInputInfo = (BOOL (__stdcall *)(__PLASTINPUTINFO))p;
		return true;
	} else {
		delete d->lib;
		d->lib = 0;
	}

	// fall back on idleui
	d->lib = new QLibrary("idleui");
	if(d->lib->load() && (p = d->lib->resolve("IdleUIGetLastInputTime"))) {
		d->IdleUIGetLastInputTime = (DWORD (__stdcall *)(void))p;
		return true;
	} else {
		delete d->lib;
		d->lib = 0;
	}

	return false;
}

int IdlePlatform::secondsIdle()
{
	int i;
	if(d->GetLastInputInfo) {
		__LASTINPUTINFO li;
		li.cbSize = sizeof(__LASTINPUTINFO);
		bool ok = d->GetLastInputInfo(&li);
		if(!ok)
			return 0;
		i = li.dwTime;
	} else if (d->IdleUIGetLastInputTime) {
		i = d->IdleUIGetLastInputTime();
	} else
		return 0;

	return (GetTickCount() - i) / 1000;
}
}

#endif

