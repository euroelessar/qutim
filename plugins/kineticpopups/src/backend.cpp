/****************************************************************************
 *  backend.cpp
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
#include "backend.h"
#include <qutim/debug.h>
#include "widgetplacer.h"
#include "popupwidgets/quickpopupwidget.h"
#include "settings/popupappearance.h"

#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace KineticPopups {

using namespace qutim_sdk_0_3;

Backend::Backend() :
	NotificationBackend("Popup"),
	m_placer(new WidgetPlacer(this))
{
	m_item = new GeneralSettingsItem<PopupAppearance>(Settings::Appearance,
													  Icon("dialog-information"),
													  QT_TRANSLATE_NOOP("Settings","Popups"));
	Settings::registerItem(m_item);

	qmlRegisterUncreatableType<Notification>("qutIM", 0, 3,
											 "Notification",
											 tr("Unable to create notification inside QtQuick"));
	qmlRegisterType<PopupAttributes>("qutIM", 0, 3, "PopupAttributes");


}

Backend::~Backend()
{
	Settings::removeItem(m_item);
	delete m_item;
}

void Backend::handleNotification(qutim_sdk_0_3::Notification *notification)
{
	ref(notification);
	if (!split(notification)) {
		QuickPopupWidget *popup = new QuickPopupWidget;
		popup->addNotification(notification);
		m_placer->addWidget(popup);
		m_activeNotifyHash.insert(popup, notification);
		connect(popup, SIGNAL(destroyed(QObject*)), SLOT(onPopupDestroyed(QObject*)));
	}
}

void Backend::onPopupDestroyed(QObject *obj)
{
	PopupWidget *popup = static_cast<PopupWidget*>(obj);
	foreach (Notification *notify, m_activeNotifyHash.values(popup)) {
		deref(notify);
		m_activeNotifyHash.remove(popup, notify);
	}
}

bool Backend::split(qutim_sdk_0_3::Notification *notify)
{
	NotificationHash::const_iterator it = m_activeNotifyHash.constBegin();
	for (; it != m_activeNotifyHash.constEnd(); it++) {
		NotificationRequest other = it.value()->request();
		NotificationRequest request = notify->request();
		if (request.object() && request.object() == other.object()
				&& request.type() == other.type()) {
			it.key()->addNotification(notify);
			return true;
		}
	}
	return false;
}

} // namespace KineticPopups
