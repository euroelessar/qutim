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
#include <qutim/icon.h>
#include <qutim/config.h>
#include <QPainter>
#include <QFont>
#include <qt_windows.h>

#include <QtWinExtras/QWinTaskbarButton>
#include <QMessageBox>

using namespace qutim_sdk_0_3;

WOverlayIcon::WOverlayIcon()
{
	reloadSettings();
	connect(WinIntegration::instance(), SIGNAL(reloadSettigs()), SLOT(reloadSettings()));
}

void WOverlayIcon::onUnreadChanged(unsigned unreadChats, unsigned unreadConfs)
{
	if (!(WinIntegration::oneOfChatWindows()))
		return;
	QWinTaskbarButton button;
	button.setWindow(WinIntegration::oneOfChatWindows()->windowHandle());
	if (!(unreadChats + unreadConfs)) {
		button.clearOverlayIcon();
		return;
	}
	quint32 count = unreadChats + (cfg_addConfs ? unreadConfs : 0);
	QPixmap icon;
	int height = GetSystemMetrics(SM_CXSMICON);
	if (unreadConfs && !unreadChats)
		icon = Icon("winoverlay-mail-message")   .pixmap(height, height);
	else
		icon = Icon("winoverlay-mail-unread-new").pixmap(height, height);
	if (icon.isNull())
		QMessageBox::information(0, "Nooo", "Invalid icon");
	QPainter painter(&icon);
	if (cfg_displayNumber && count) {
		QFont font;
		font.setWeight(QFont::DemiBold);
		font.setFamily("Segoe UI");
		font.setPointSize(7);
		painter.setFont(font);
		painter.setPen(Qt::darkBlue);
		painter.drawText(QRect(0, 0, 16, 16), Qt::AlignCenter, QString::number(count));
	}
	button.setOverlayIcon(icon);
}

void WOverlayIcon::reloadSettings()
{
	if (!(WinIntegration::oneOfChatWindows()))
		return;
	QWinTaskbarButton button;
	button.setWindow(WinIntegration::oneOfChatWindows()->windowHandle());
	Config cfg(WI_ConfigName);
	cfg_addConfs      = cfg.value("oi_addNewConfMsgNumber", false);
	cfg_displayNumber = cfg.value("oi_showNewMsgNumber",    true);
	cfg_enabled       = cfg.value("oi_enabled",             true);
	if (cfg_enabled) {
		connect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), this, SLOT(onUnreadChanged(uint,uint)), Qt::UniqueConnection);
	} else {
		disconnect(WinIntegration::instance(), SIGNAL(unreadChanged(uint,uint)), this, SLOT(onUnreadChanged(uint,uint)));
		button.clearOverlayIcon();
	}
}
