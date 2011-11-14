/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Nokia Corporation
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

#ifndef QUTIMWIN_H
#define QUTIMWIN_H
#include "libqutim_global.h"

class QColor;
namespace qutim_sdk_0_3
{
	/**
	* @brief This is a helper namespace for using the Desktop Window Manager
	* functionality on Windows 7 and Windows Vista. On other platforms
	* these functions will simply not do anything.
	*/
	namespace QtWin {
		/*!
		* Enables Blur behind on a Widget.
		*
		* \a enable tells if the blur should be enabled or not
		*/		
		LIBQUTIM_EXPORT bool enableBlurBehindWindow(QWidget *widget, bool enable = true);
		/*!
		* ExtendFrameIntoClientArea.
		*
		* This controls the rendering of the frame inside the window.
		* Note that passing margins of -1 (the default value) will completely
		* remove the frame from the window.
		*
		* \note you should not call enableBlurBehindWindow before calling
		*       this functions
		*
		* \a enable tells if the blur should be enabled or not
		*/
		LIBQUTIM_EXPORT bool extendFrameIntoClientArea(QWidget *widget,
													   int left = -1, int top = -1,
													   int right = -1, int bottom = -1);
		/*!
		* @brief Chekcs and returns true if Windows DWM composition
		* is currently enabled on the system.
		*
		* @note To get live notification on the availability of
		* this feature, you will currently have to
		* reimplement winEvent() on your widget and listen
		* for the WM_DWMCOMPOSITIONCHANGED event to occur.
		*
		*@return current composition status
		*
		*/									
		LIBQUTIM_EXPORT bool isCompositionEnabled();
		/*!
		* Returns the current colorizationColor for the window.
		*
		* \a enable tells if the blur should be enabled or not
		*/		
		LIBQUTIM_EXPORT QColor colorizationColor();
	}
	
}

#endif // QUTIMWIN_H

