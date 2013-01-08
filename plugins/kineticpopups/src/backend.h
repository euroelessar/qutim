/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef CORE_KINETICPOPUPS_BACKEND_H
#define CORE_KINETICPOPUPS_BACKEND_H
#include <qutim/notification.h>
#include <QHash>

namespace qutim_sdk_0_3 {
class SettingsItem;
}

namespace KineticPopups {

class PopupWidget;
typedef QMultiHash<PopupWidget*, QPointer<qutim_sdk_0_3::Notification> > NotificationHash;

class WidgetPlacer;
class Backend : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Popup")
	Q_CLASSINFO("Uses", "SettingsLayer")
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

