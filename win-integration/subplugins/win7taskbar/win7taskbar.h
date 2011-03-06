#ifndef WIN7TASKBAR_H
#define WIN7TASKBAR_H

#include <qutim/plugin.h>

class WThumbnails;
class WOverlayIcon;

class Win7Features : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "WinIntegration/Win7Features")
	Q_CLASSINFO("DependsOn", "WinIntegration")
	Q_CLASSINFO("Service",   "Dock")

public:
	void init();
	bool load();
	bool unload();

private:
	WThumbnails *thumbnails;
	WOverlayIcon *overlayIcon;
};

#endif // WIN7TASKBAR_H
