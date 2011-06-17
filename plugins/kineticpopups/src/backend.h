/****************************************************************************
 *  backend.h
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

#ifndef CORE_KINETICPOPUPS_BACKEND_H
#define CORE_KINETICPOPUPS_BACKEND_H
#include <qutim/notification.h>
#include <QHash>

namespace qutim_sdk_0_3 {
class SettingsItem;
}

namespace KineticPopups {

class PopupWidget;
typedef QMultiHash<PopupWidget*, qutim_sdk_0_3::Notification*> NotificationHash;

class WidgetPlacer;
class Backend : public qutim_sdk_0_3::NotificationBackend
{
	Q_CLASSINFO("Service", "Popup")
	Q_CLASSINFO("Uses", "SettingsLayer")
	Q_OBJECT
public:
	Backend();
	virtual ~Backend();
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
protected slots:
	void onPopupDestroyed(QObject *obj);
	bool split(qutim_sdk_0_3::Notification *notify);
private:
	WidgetPlacer *m_placer;
	NotificationHash m_activeNotifyHash;
	qutim_sdk_0_3::SettingsItem *m_item;
};

} // namespace KineticPopups

#endif // CORE_KINETICPOPUPS_BACKEND_H
