/****************************************************************************
**
** Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#include "qtdwm.h"
#include "qtdwm_p.h"
#include <QLibrary>
#include <QApplication>
#include <QWidget>
#include <QList>

#ifdef Q_WS_WIN

#include <qt_windows.h>

// Blur behind data structures
#define DWM_BB_ENABLE                 0x00000001  // fEnable has been specified
#define DWM_BB_BLURREGION             0x00000002  // hRgnBlur has been specified
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004  // fTransitionOnMaximized has been specified
#define WM_DWMCOMPOSITIONCHANGED        0x031E    // Composition changed window message

typedef struct _DWM_BLURBEHIND
{
	DWORD dwFlags;
	BOOL fEnable;
	HRGN hRgnBlur;
	BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

typedef struct _MARGINS
{
	int cxLeftWidth;
	int cxRightWidth;
	int cyTopHeight;
	int cyBottomHeight;
} MARGINS, *PMARGINS;

typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS* pMarInset);
typedef HRESULT (WINAPI *PtrDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
typedef HRESULT (WINAPI *PtrDwmGetColorizationColor)(DWORD *pcrColorization, BOOL *pfOpaqueBlend);

static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled= 0;
static PtrDwmEnableBlurBehindWindow pDwmEnableBlurBehindWindow = 0;
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;
static PtrDwmGetColorizationColor pDwmGetColorizationColor = 0;


/*
  Used to reenable blur or reextend frame on composition toggling.
*/
class BlurManager
{
public:
	BlurManager(QWidget *p)
	{
		m_widgetPtr = p;
	}

	virtual ~BlurManager(){}
	virtual void reenable() = 0;

protected:
	QWidget *m_widgetPtr;
};

class BlurBehindManager : public BlurManager
{
public:
	BlurBehindManager(QWidget *w, const DWM_BLURBEHIND *bbStruct)
		: BlurManager(w)
	{
		memcpy(&BlurBehindStruct, bbStruct, sizeof(DWM_BLURBEHIND));
		if (BlurBehindStruct.hRgnBlur)
			CombineRgn(BlurBehindStruct.hRgnBlur, bbStruct->hRgnBlur, NULL, RGN_COPY);
	}

	virtual void reenable()
	{
		pDwmEnableBlurBehindWindow(m_widgetPtr->winId(), &BlurBehindStruct);
	}

	~BlurBehindManager()
	{
		DeleteObject(BlurBehindStruct.hRgnBlur);
	}

private:
	DWM_BLURBEHIND BlurBehindStruct;
};

class ExtendedFrameManager : public BlurManager
{
public:
	ExtendedFrameManager(QWidget *w, MARGINS *m)
		: BlurManager(w)
	{
		memcpy(&margins, m, sizeof(MARGINS));
	}

	virtual void reenable()
	{
		pDwmExtendFrameIntoClientArea(m_widgetPtr->winId(), &margins);
	}

private:
	MARGINS margins;
};

WindowNotifier::WindowNotifier()
{
	winId();
}

void WindowNotifier::addWidget(QWidget *widget, BlurManager *bm)
{
	widgets[widget] = bm;
	connect(widget, SIGNAL(destroyed(QObject*)), SLOT(removeWidget(QObject*)));
}

void WindowNotifier::removeWidget(QObject *widget)
{
	QWidget *key = static_cast<QWidget *>(widget);
	delete widgets[key];
	widgets.remove(key);
}

/* Notify all enabled windows that the DWM state changed */
bool WindowNotifier::winEvent(MSG *message, long *result)
{
	if (message && message->message == WM_DWMCOMPOSITIONCHANGED) {
		bool compositionEnabled = QtDWM::isCompositionEnabled();
		WidgetsMap::iterator i = widgets.begin();
		while (i != widgets.end()) {
			QWidget * w = i.key();
			if (w) {
				w->setAttribute(Qt::WA_NoSystemBackground, compositionEnabled);
				w->update();
				i.value()->reenable();
			}
			++i;
		}
	}
	return QWidget::winEvent(message, result);
}

static bool resolveLibs()
{
	if (!pDwmIsCompositionEnabled) {
		QLibrary dwmLib(QString::fromAscii("dwmapi"));
		pDwmIsCompositionEnabled =(PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
		pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
		pDwmEnableBlurBehindWindow = (PtrDwmEnableBlurBehindWindow)dwmLib.resolve("DwmEnableBlurBehindWindow");
		pDwmGetColorizationColor = (PtrDwmGetColorizationColor)dwmLib.resolve("DwmGetColorizationColor");
	}
	return pDwmIsCompositionEnabled != 0;
}

WindowNotifier *QtDWM::windowNotifier()
{
	 static WindowNotifier *windowNotifierInstance = 0;
	 if (!windowNotifierInstance)
		  windowNotifierInstance = new WindowNotifier;
	 return windowNotifierInstance;
}

#endif

/*!
  * Chekcs and returns true if Windows DWM composition
  * is currently enabled on the system.
  *
  * To get live notification on the availability of
  * this feature, you will currently have to
  * reimplement winEvent() on your widget and listen
  * for the WM_DWMCOMPOSITIONCHANGED event to occur.
  *
  */
bool QtDWM::isCompositionEnabled()
{
#ifdef Q_WS_WIN
	if (resolveLibs()) {
		HRESULT hr = S_OK;
		BOOL isEnabled = false;
		hr = pDwmIsCompositionEnabled(&isEnabled);
		if (SUCCEEDED(hr))
			return isEnabled;
	}
#endif
	return false;
}

/*!
  * Enables Blur behind on a Widget.
  *
  * \a enable tells if the blur should be enabled or not
  */
bool QtDWM::enableBlurBehindWindow(QWidget *widget, bool enable)
{
	Q_UNUSED(enable);
	Q_ASSERT(widget);
	bool result = false;
#ifdef Q_WS_WIN
	if (resolveLibs()) {
		DWM_BLURBEHIND bb = {0, 0, 0, 0};
		HRESULT hr = S_OK;
		bb.fEnable = enable;
		bb.dwFlags = DWM_BB_ENABLE;
		bb.hRgnBlur = NULL;
		widget->setAttribute(Qt::WA_TranslucentBackground, enable);
		widget->setAttribute(Qt::WA_NoSystemBackground, enable);
		hr = pDwmEnableBlurBehindWindow(widget->winId(), &bb);
		if (SUCCEEDED(hr)) {
			result = true;
				windowNotifier()->addWidget(widget, new BlurBehindManager(widget, &bb));
		}
	}
#else
	Q_UNUSED(widget);
	Q_UNUSED(enable);
#endif
	return result;
}

/*!
  * ExtendFrameIntoClientArea.
  *
  * This controls the rendering of the frame inside the window.
  * Note that passing margins of -1 (the default value) will completely
  * remove the frame from the window.
  *
  * \note you should not call enableBlurBehindWindow before calling
  *	   this functions
  *
  * \a enable tells if the blur should be enabled or not
  */
bool QtDWM::extendFrameIntoClientArea(QWidget *widget, int left, int top, int right, int bottom)
{
	Q_ASSERT(widget);
	bool result = false;
#ifdef Q_WS_WIN
	if (resolveLibs()) {
		QLibrary dwmLib(QString::fromAscii("dwmapi"));
		HRESULT hr = S_OK;
		MARGINS m = {left, top, right, bottom};
		hr = pDwmExtendFrameIntoClientArea(widget->winId(), &m);
		if (SUCCEEDED(hr)) {
			result = true;
				windowNotifier()->addWidget(widget, new ExtendedFrameManager(widget, &m));
		}
		widget->setAttribute(Qt::WA_TranslucentBackground, result);
	}
#else
	Q_UNUSED(widget);
	Q_UNUSED(left);
	Q_UNUSED(top);
	Q_UNUSED(right);
	Q_UNUSED(bottom);
#endif
	return result;
}

/*!
  * Returns the current colorizationColor for the window.
  *
  * \a enable tells if the blur should be enabled or not
  */
QColor QtDWM::colorizationColor()
{
	QColor resultColor = QApplication::palette().window().color();

#ifdef Q_WS_WIN
	if (resolveLibs()) {
		DWORD color = 0;
		BOOL opaque = FALSE;
		QLibrary dwmLib(QString::fromAscii("dwmapi"));
		HRESULT hr = S_OK;
		hr = pDwmGetColorizationColor(&color, &opaque);
		if (SUCCEEDED(hr))
			resultColor = QColor(color);
	}
#endif
	return resultColor;
}
