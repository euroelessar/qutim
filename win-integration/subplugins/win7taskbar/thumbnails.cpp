#include "thumbnails.h"
#include "thumbnails-renderer.h"
#include "../../src/winint.h"
#include <WinThings/TaskbarPreviews.h>
#include <qutim/messagesession.h>

using namespace qutim_sdk_0_3;

WThumbnails::WThumbnails()
{
	pp         = new WThumbnailsProvider(this);
	chatWindow = 0;
	tabId      = 0;
	connect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), pp, SLOT(onUnreadChanged(uint,uint)));
	connect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), SLOT(onUnreadChanged(uint,uint)));
}

WThumbnails::~WThumbnails()
{
	delete pp;
	TaskbarPreviews::tabsClear();
}

void WThumbnails::onChatwindowDestruction(QObject *)
{
	TaskbarPreviews::tabsClear();
	tabId      = 0;
	chatWindow = 0;
}

void WThumbnails::onSessionCreated(qutim_sdk_0_3::ChatSession *s)
{
	connect(s, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
	connect(s, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), pp, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
}

void WThumbnails::onSessionActivated(bool)
{
	bool newWindow = false;
	if (!chatWindow) {
		chatWindow = WinIntegration::oneOfChatWindows();
		newWindow  = true;
	}
	if (!chatWindow)
		return;
	if (newWindow) {
		tabId = TaskbarPreviews::tabAddVirtual(pp, chatWindow, chatWindow->windowTitle());
		connect(chatWindow, SIGNAL(destroyed(QObject*)), SLOT(onChatwindowDestruction(QObject*)));
	} else
		TaskbarPreviews::tabSetTitle(tabId, chatWindow->windowTitle());
}

void WThumbnails::onUnreadChanged(unsigned, unsigned)
{
	TaskbarPreviews::tabPreviewsRefresh(tabId);
}

QWidget *WThumbnails::currentWindow()
{
	return chatWindow;
}
