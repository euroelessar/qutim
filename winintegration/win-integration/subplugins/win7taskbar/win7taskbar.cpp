#include "win7taskbar.h"
#include "thumbnails.h"
#include "overlay-icon.h"
#include "../../src/winint.h"
#include <QSysInfo>
#include <qutim/messagesession.h>
#include <qutim/servicemanager.h>
#include <WinThings/TaskbarPreviews.h>

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
	//connect(WinIntegration::instance(), SIGNAL(reloadSettigs()), thumbnails,  SLOT(reloadSetting())); // no need - loads them once
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), thumbnails, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	TaskbarPreviews::setWindowAttributes(ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>(), TA_Flip3D_ExcludeBelow | TA_Peek_ExcludeFrom);
	WinIntegration::instance()->enabledPlugin(WI_Win7Taskbar);
	return true;
}

bool Win7Features::unload()
{
	if (QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7)
		return false;
	delete thumbnails;
	delete overlayIcon;
	thumbnails  = 0;
	overlayIcon = 0;
	TaskbarPreviews::setWindowAttributes(ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>(), TA_NoAttributes);
	WinIntegration::instance()->disabledPlugin(WI_Win7Taskbar);
	return true;
}

QUTIM_EXPORT_PLUGIN(Win7Features)
