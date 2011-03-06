#include "win7taskbar.h"
#include "thumbnails.h"
#include "overlay-icon.h"
#include "../../src/winint.h"
#include <QSysInfo>
#include <qutim/messagesession.h>

using namespace qutim_sdk_0_3;

void Win7Features::init()
{
	thumbnails  = 0;
	overlayIcon = 0;
}

bool Win7Features::load()
{
	if (QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7)
		return false;
	thumbnails  = new WThumbnails;
	overlayIcon = new WOverlayIcon;
	connect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), overlayIcon, SLOT(onUnreadChanged(uint,uint)));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), thumbnails, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	return true;
}

bool Win7Features::unload()
{
	delete thumbnails;
	delete overlayIcon;
	thumbnails  = 0;
	overlayIcon = 0;
	return true;
}

QUTIM_EXPORT_PLUGIN(Win7Features)
