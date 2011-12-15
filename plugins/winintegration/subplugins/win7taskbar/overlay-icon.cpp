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

#include "overlay-icon.h"
#include "../../src/winint.h"
#include <WinThings/OverlayIcon.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <QPainter>
#include <QFont>

using namespace qutim_sdk_0_3;

WOverlayIcon::WOverlayIcon()
{
	reloadSettings();
	connect(WinIntegration::instance(), SIGNAL(reloadSettigs()), SLOT(reloadSettings()));
}

void WOverlayIcon::onUnreadChanged(unsigned unreadChats, unsigned unreadConfs)
{
	if (!(unreadChats + unreadConfs)) {
		OverlayIcon::clear(WinIntegration::oneOfChatWindows());
		return;
	}
	quint32 count = unreadChats + (cfg_addConfs ? unreadConfs : 0);
	QPixmap icon;
	if(unreadConfs && !unreadChats)
		icon = Icon("mail-message")   .pixmap(16, 16);
	else
		icon = Icon("mail-unread-new").pixmap(16, 16);
	QPainter painter(&icon);
	if(cfg_displayNumber && count){
		QFont font;
		font.setWeight(QFont::DemiBold);
		font.setFamily("Segoe UI");
		font.setPointSize(7);
		painter.setFont(font);
		painter.setPen(Qt::darkBlue);
		painter.drawText(QRect(0, 1, 15, 15), Qt::AlignCenter, QString::number(count));
	}
	OverlayIcon::set(WinIntegration::oneOfChatWindows(), icon);
}

void WOverlayIcon::reloadSettings()
{
	Config cfg(WI_ConfigName);
	cfg_addConfs      = cfg.value("oi_addNewConfMsgNumber", false);
	cfg_displayNumber = cfg.value("oi_showNewMsgNumber",    true);
	cfg_enabled       = cfg.value("oi_enabled",             true);
	if (cfg_enabled) {
		connect   (WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), this, SLOT(onUnreadChanged(uint,uint)), Qt::UniqueConnection);
	} else {
		disconnect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), this, SLOT(onUnreadChanged(uint,uint)));
		OverlayIcon::clear(WinIntegration::oneOfChatWindows());
	}
}

