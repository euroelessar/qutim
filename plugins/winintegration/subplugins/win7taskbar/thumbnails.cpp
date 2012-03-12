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

#include "thumbnails.h"
#include "thumbnails-renderer.h"
#include "../../src/winint.h"
#include <WinThings/TaskbarPreviews.h>
#include <qutim/chatsession.h>
#include <qutim/config.h>
#include <QTimer>

using namespace qutim_sdk_0_3;

WThumbnails::WThumbnails()
	: chatWindow(0), tabId(0), pp(0)
{
	reloadSetting();
	if (cfg_enabled) {
		pp = new WThumbnailsProvider(this);
		pp->reloadSettings();
		connect(WinIntegration::instance(), SIGNAL(reloadSettigs()),          pp, SLOT(reloadSettings()));
		connect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), pp, SLOT(onUnreadChanged(uint,uint)));
		connect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)),     SLOT(onUnreadChanged(uint,uint)));
	}
}

WThumbnails::~WThumbnails()
{
	if (pp)
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
	} else {
		TaskbarPreviews::tabSetTitle(tabId, chatWindow->windowTitle());
		QTimer::singleShot(0, pp, SLOT(prepareLivePreview()));
	}
}

void WThumbnails::onUnreadChanged(unsigned, unsigned)
{
	TaskbarPreviews::tabPreviewsRefresh(tabId);
}

QWidget *WThumbnails::currentWindow()
{
	return chatWindow;
}

void WThumbnails::reloadSetting()
{
	Config cfg(WI_ConfigName);
	cfg_enabled         = cfg.value("tt_enabled", true);
	cfg_showLastSenders = cfg.value("tt_showLastSenders",  true);
	cfg_showMsgNumber   = cfg.value("tt_showNewMsgNumber", true);
}

