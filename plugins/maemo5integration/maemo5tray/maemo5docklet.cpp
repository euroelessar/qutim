/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "maemo5docklet.h"
#include "maemo5dockletconnector.h"
#include <QWidget>
#include <QStyle>

Maemo5Docklet* Maemo5Docklet::m_instance = 0;

Maemo5Docklet::Maemo5Docklet() : QObject(0)
{
}

Maemo5Docklet::~Maemo5Docklet()
{
}

Maemo5Docklet* Maemo5Docklet::Instance()
{
	if (m_instance == 0)
		m_instance = new Maemo5Docklet;
	return m_instance;
}

void Maemo5Docklet::Enable()
{
	qutim_status_menu_plugin_connect();
}

void Maemo5Docklet::Disable()
{
	qutim_status_menu_plugin_disconnect();
}

void Maemo5Docklet::SetIcon(const QString &iconPath, bool blink )
{
	qutim_status_menu_plugin_update_icon(iconPath.toLatin1().data(),blink);
}

void Maemo5Docklet::SetTooltip(const QString &tooltip )
{
	qutim_status_menu_set_tooltip((gchar*)(tooltip.toLatin1().data()));
}

void Maemo5Docklet::SetMute(bool is_muted)
{
	mute_changed_cb(is_muted);
}

void Maemo5Docklet::clickedSignal()
{
	emit ButtonClicked();
}

void Maemo5Docklet::ButtonClickedCallback()
{
	Instance()->clickedSignal();
}

static int icon_size;

int Maemo5Docklet::GetIconSize()
{
	if (icon_size==0)
	{
		QWidget w;
		icon_size = w.style()->pixelMetric(QStyle::PM_ToolBarIconSize);
	}
	return icon_size;
}


