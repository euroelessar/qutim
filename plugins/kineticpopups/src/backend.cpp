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
#include "settings/popupappearance.h"
#include "notifylist.h"
#include "notify.h"

#include <qutim/declarativeview.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/thememanager.h>

namespace KineticPopups {

using namespace qutim_sdk_0_3;

static Backend *backend_instance = nullptr;

Backend::Backend() :
	NotificationBackend("Popup"),
    m_component(DeclarativeView::globalEngine())
{
    backend_instance = this;

	setDescription(QT_TR_NOOP("Show popup"));
	m_item = new GeneralSettingsItem<PopupAppearance>(Settings::Appearance,
													  Icon("dialog-information"),
													  QT_TRANSLATE_NOOP("Settings","Popups"));
	m_item->connect(SIGNAL(saved()), this, SLOT(onSettingsSaved()));
	Settings::registerItem(m_item);
    
    qmlRegisterUncreatableType<Action>("org.qutim.kineticpopups", 0, 4,
	                                   "Action",
	                                   "Unable to create action inside QtQuick");
	qmlRegisterUncreatableType<Notify>("org.qutim.kineticpopups", 0, 4,
	                                   "Notify",
	                                   "Unable to create notify inside QtQuick");
	qmlRegisterType<NotifyList>("org.qutim.kineticpopups", 0, 4, "NotifyList");

	onSettingsSaved();
}

Backend::~Backend()
{
    backend_instance = nullptr;

	Settings::removeItem(m_item);
	delete m_item;
}

void Backend::handleNotification(qutim_sdk_0_3::Notification *notification)
{
	m_notifications.append(notification);
	ref(notification);

    connect(notification, &Notification::finished, this, &Backend::onNotificationFinished);

    emit notificationAdded(notification);
}

void Backend::onNotificationFinished()
{
    if (Notification *notify = qobject_cast<Notification *>(sender()))
        removeNotification(notify);
}

Backend *Backend::instance()
{
    return backend_instance;
}

void Backend::removeNotification(Notification *notification)
{
	if (m_notifications.removeOne(notification)) {
		emit notificationRemoved(notification);
		deref(notification);
	}
}

QList<Notification *> Backend::notifications() const
{
	return m_notifications;
}

void Backend::onSettingsSaved()
{
	Config cfg(QStringLiteral("behavior"));
	cfg.beginGroup(QStringLiteral("popup"));

	QString themeName = cfg.value(QStringLiteral("themeName"), QStringLiteral("default"));
	QString themePath = ThemeManager::path(QStringLiteral("quickpopup"), themeName);
	QString fileName = themePath + QStringLiteral("/main.qml");
	QUrl themeUrl = QUrl::fromLocalFile(fileName);

	if (themeUrl != m_component.url()) {
		m_component.loadUrl(themeUrl);
		m_logic.reset(m_component.create());
        
        if (!m_logic)
            qDebug() << "Failed to load" << themeUrl << m_component.errorString();
	}

//	m_timeout.setInterval(cfg.value(QStringLiteral("timeout"), 5) * 1000);
}

//bool Backend::split(qutim_sdk_0_3::Notification *notify)
//{
//	NotificationHash::const_iterator it = m_activeNotifyHash.constBegin();
//	for (; it != m_activeNotifyHash.constEnd(); ++it) {
//		if (!it.value())
//			continue;
//		NotificationRequest other = it.value().data()->request();
//		NotificationRequest request = notify->request();
//		if (request.object() && request.object() == other.object()
//				&& request.type() == other.type()) {
//			it.key()->addNotification(notify);
//			return true;
//		}
//	}
//	return false;
//}

} // namespace KineticPopups

