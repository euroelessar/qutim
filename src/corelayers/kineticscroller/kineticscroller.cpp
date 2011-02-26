/****************************************************************************
 *  kineticscroller.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "kineticscroller.h"
#include <qtscroller.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/config.h>

namespace Core {

using namespace qutim_sdk_0_3;

KineticScroller::KineticScroller()
{
	debug() << Q_FUNC_INFO;
	AutoSettingsItem *settingsItem = new AutoSettingsItem(Settings::Plugin,
														  Icon("applications-system"),
														  QT_TRANSLATE_NOOP("Plugin", "Kinetic scrolling"));
	settingsItem->setConfig(QString(),"kineticScrolling");

	QStringList list;
	list.append(tr("No scrolling"));
	list.append(tr("Touch"));
	list.append(tr("Left mouse button"));
	list.append(tr("Middle mouse button"));
	list.append(tr("Right mouse button"));
	AutoSettingsItem::Entry *entry = settingsItem->addEntry<AutoSettingsComboBox>(QT_TRANSLATE_NOOP("Settings", "Scrolling type"));
	entry->setName("type");
	entry->setProperty("items", list);
	Settings::registerItem(settingsItem);
	loadSettings();
}

KineticScroller::~KineticScroller()
{

}

void KineticScroller::enableScrolling(QObject *widget)
{
	if (m_scrollingType != -1)
		QtScroller::grabGesture(widget, static_cast<QtScroller::ScrollerGestureType>(m_scrollingType));
}

void KineticScroller::loadSettings()
{
	m_scrollingType = Config().group("kineticScrolling").value("type", QtScroller::LeftMouseButtonGesture + 1) - 1;
	debug() << m_scrollingType;
}

} // namespace Core
