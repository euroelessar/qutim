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

#include <windows.h>

void RegisterTab              (HWND tab, HWND owner);
void SetTabActive             (HWND tab, HWND owner);
void SetNoTabActive           (HWND owner); // SetTabActive(0, owner)
void SetTabOrder              (HWND tab, HWND insertBefore);
void UnregisterTab            (HWND tab);
void SetTabIconicPreview      (HWND tab, HBITMAP bitmap);//
void SetTabLivePreview        (HWND tab, HBITMAP bitmap);//
void ForceIconicRepresentation(HWND tab);
void InvalidateBitmaps        (HWND hwnd);

