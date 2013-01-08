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
	setDescription(QT_TR_NOOP("Show popup"));
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
		connect(popup, SIGNAL(finished()), popup, SLOT(deleteLater()));
	}
}

void Backend::onPopupDestroyed(QObject *obj)
{
	PopupWidget *popup = static_cast<PopupWidget*>(obj);
	foreach (QPointer<Notification> notify, m_activeNotifyHash.values(popup)) {
		if (notify)
			deref(notify.data());
	}
	m_activeNotifyHash.remove(popup);
}

bool Backend::split(qutim_sdk_0_3::Notification *notify)
{
	NotificationHash::const_iterator it = m_activeNotifyHash.constBegin();
	for (; it != m_activeNotifyHash.constEnd(); ++it) {
		if (!it.value())
			continue;
		NotificationRequest other = it.value().data()->request();
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

