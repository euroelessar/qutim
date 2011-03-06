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
	if(cfg_displayCount && count){
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
	Config cfg(WI_CONFIG);
	cfg_addConfs     = cfg.value("oi_addConfs",     false);
	cfg_displayCount = cfg.value("oi_displayCount", true);
}
