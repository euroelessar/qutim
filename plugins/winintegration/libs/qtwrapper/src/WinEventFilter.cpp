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

#include "WinEventFilter.h"
#include "TaskbarPreviews.h"
#include "../../apilayer/src/ApiTaskbarPreviews.h"
#include <QPixmap>
#include <qt_windows.h>
#include <QDebug>
#include <QtWinExtras/QtWinExtras>

#ifndef WM_DWMSENDICONICTHUMBNAIL
#	define WM_DWMSENDICONICTHUMBNAIL           0x0323
#endif

#ifndef WM_DWMSENDICONICLIVEPREVIEWBITMAP
#	define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326
#endif

Win7EventFilter *Win7EventFilter::instance()
{
	static Win7EventFilter _instance;
	return &_instance;
}

Win7EventFilter::Win7EventFilter()
{
	qApp->installNativeEventFilter(this);
}

bool Win7EventFilter::nativeEventFilter(const QByteArray &, void * message, long * result) 
{
	MSG *msg = static_cast<MSG *>(message);
	//qDebug() << *msg;
	switch(msg->message)
	{
	case WM_DWMSENDICONICTHUMBNAIL : {
		HBITMAP bmp = QtWin::toHBITMAP(TaskbarPreviews::instance()
						  ->IconicThumbnail(msg->hwnd, QSize(HIWORD(msg->lParam), LOWORD(msg->lParam)))
						  , QtWin::HBitmapAlpha);
		SetTabIconicPreview(msg->hwnd, bmp);
		DeleteObject(bmp);
		if (result)
			*result = 0;
		return true;
	}
	case WM_DWMSENDICONICLIVEPREVIEWBITMAP : {
		HBITMAP bmp = QtWin::toHBITMAP(TaskbarPreviews::instance()
						  ->IconicLivePreviewBitmap(msg->hwnd)
						  , QtWin::HBitmapAlpha);
		SetTabLivePreview(msg->hwnd, bmp);
		DeleteObject(bmp);
		if (result)
			*result = 0;
		return true;
	}
	case WM_ACTIVATE : {
		if(TaskbarPreviews::instance()->WasTabActivated(msg->hwnd)){
			if (result)
				*result = 0;
			return true;
		}
	}
	case WM_CLOSE : {
		if(TaskbarPreviews::instance()->WasTabRemoved(msg->hwnd)){
			if (result)
				*result = 0;
			return true;
		}
	}
	}
	return false;
}

