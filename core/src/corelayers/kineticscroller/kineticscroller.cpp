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
#include <QAbstractItemView>

namespace Core {

using namespace qutim_sdk_0_3;

KineticScroller::KineticScroller()
{
	debug() << Q_FUNC_INFO;
	AutoSettingsItem *settingsItem = new AutoSettingsItem(Settings::Plugin,
														  Icon("applications-system"),
														  QT_TRANSLATE_NOOP("Plugin", "Kinetic scrolling"));
	settingsItem->setConfig(QString(),"kineticScrolling");
	settingsItem->connect(SIGNAL(saved()), this, SLOT(loadSettings()));

	QStringList list;
	//list.append(tr("No scrolling"));
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
	if (m_widgets.contains(widget))
		return;
	m_widgets.insert(widget);
	connect(widget, SIGNAL(destroyed(QObject*)), SLOT(onWidgetDeath(QObject*)));
	if (m_scrollingType != -1) {
		QtScroller::grabGesture(widget, static_cast<QtScroller::ScrollerGestureType>(m_scrollingType));
		if (QAbstractItemView *view = qobject_cast<QAbstractItemView*>(widget->parent()))
			view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	}
}

void KineticScroller::loadSettings()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("kineticScrolling"));
#if defined(Q_WS_S60)
	m_scrollingType = QtScroller::LeftMouseButtonGesture;
#else
	m_scrollingType = QtScroller::TouchGesture;
#endif

	m_scrollingType = cfg.value(QLatin1String("type"), m_scrollingType);
	QVariantList vars = cfg.value(QLatin1String("properties"), QVariantList());
	QtScrollerProperties::unsetDefaultScrollerProperties();
	QtScrollerProperties props;
	for (int i = 0; i < QtScrollerProperties::ScrollMetricCount; i++) {
		QtScrollerProperties::ScrollMetric id = static_cast<QtScrollerProperties::ScrollMetric>(i);
		QVariant var = vars.value(i, props.scrollMetric(id));
		props.setScrollMetric(id, var);
	}
	QtScrollerProperties::setDefaultScrollerProperties(props);
	QtScroller::ScrollerGestureType gesture = static_cast<QtScroller::ScrollerGestureType>(m_scrollingType);
	QSetIterator<QObject*> it(m_widgets);
	while (it.hasNext()) {
		QObject *widget = it.next();
		//if (QtScroller::grabbedGesture(widget) != gesture) {
			if (m_scrollingType == -1)
				QtScroller::ungrabGesture(widget);
			else
				QtScroller::grabGesture(widget, gesture);
		//}
		QtScroller *scroller = QtScroller::scroller(widget);
		scroller->setScrollerProperties(props);
	}
	debug() << m_scrollingType;
}

void KineticScroller::onWidgetDeath(QObject *widget)
{
	m_widgets.remove(widget);
}

} // namespace Core
