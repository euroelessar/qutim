#include "qtwin.h"
#include <3rdparty/qtdwm/qtdwm.h>
#include <QColor>

namespace qutim_sdk_0_3 {

	namespace QtWin {
		
		bool isCompositionEnabled()
		{
			return QtDWM::isCompositionEnabled();
		}

		QColor colorizatinColor()
		{
			return QtDWM::colorizatinColor();
		}

		bool enableBlurBehindWindow ( QWidget* widget, bool enable )
		{
			return QtDWM::enableBlurBehindWindow(widget,enable);
		}

		bool extendFrameIntoClientArea ( QWidget* widget, int left, int top, int right, int bottom )
		{
			return QtDWM::extendFrameIntoClientArea(widget,left,top,right,bottom);
		}

	}
}
