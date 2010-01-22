#include "qtwin.h"
#include <3rdparty/qtwin/qtwin.h>
#include <QColor>

namespace qutim_sdk_0_3 {
	namespace QtWin {
		
		bool isCompositionEnabled()
		{
			return QtWin::isCompositionEnabled();
		}

		QColor colorizatinColor()
		{
			return QtWin::colorizatinColor();
		}

		bool enableBlurBehindWindow ( QWidget* widget, bool enable )
		{
			return QtWin::enableBlurBehindWindow(widget,enable);
		}

		bool extendFrameIntoClientArea ( QWidget* widget, int left, int top, int right, int bottom )
		{
			return QtWin::extendFrameIntoClientArea(widget,left,top,right,bottom);
		}

	}
}
