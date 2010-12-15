#include "WinEventFilter.h"
#include "TaskbarPreviews.h"
#include "../../apilayer/src/ApiTaskbarPreviews.h"
#include <QPixmap>
#include <qt_windows.h>
#include <QDebug>

#ifndef WM_DWMSENDICONICTHUMBNAIL
#	define WM_DWMSENDICONICTHUMBNAIL 0x0323
#endif

#ifndef WM_DWMSENDICONICLIVEPREVIEWBITMAP
#	define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326
#endif

QCoreApplication::EventFilter Win7EventFilter::replacedFilter = 0;

Win7EventFilter *Win7EventFilter::instance()
{
	static Win7EventFilter _instance;
	return &_instance;
}

Win7EventFilter::Win7EventFilter()
{
	this->replacedFilter = qApp->setEventFilter(&Win7EventFilter::eventFilter);
}

bool Win7EventFilter::eventFilter(void *message, long *result)
{
	MSG *msg = static_cast<MSG *>(message);
	//qDebug() << *msg;
	switch(msg->message)
	{
	case WM_DWMSENDICONICTHUMBNAIL : {
		HBITMAP bmp = TaskbarPreviews::instance()
						  ->IconicThumbnail(msg->hwnd, QSize(HIWORD(msg->lParam), LOWORD(msg->lParam)))
						  .toWinHBITMAP(QPixmap::Alpha);
		SetTabIconicPreview(msg->hwnd, bmp);
		DeleteObject(bmp);
		*result = 0;
		return true;
	}
	case WM_DWMSENDICONICLIVEPREVIEWBITMAP : {
		HBITMAP bmp = TaskbarPreviews::instance()
						  ->IconicLivePreviewBitmap(msg->hwnd)
						  .toWinHBITMAP(QPixmap::Alpha);
		SetTabLivePreview(msg->hwnd, bmp);
		DeleteObject(bmp);
		*result = 0;
		return true;
	}
	case WM_ACTIVATE : {
		if(TaskbarPreviews::instance()->WasTabActivated(msg->hwnd)){
			*result = 0;
			return true;
		}
	}
	case WM_CLOSE : {
		qDebug() << "WM_CLOSE";
		if(TaskbarPreviews::instance()->WasTabRemoved(msg->hwnd)){
			*result = 0;
			return true;
		}
	}
	}

	if(Win7EventFilter::replacedFilter)
		return Win7EventFilter::replacedFilter(message, result);
	else
		return false;
}
