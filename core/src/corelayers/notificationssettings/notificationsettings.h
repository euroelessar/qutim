/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef CORE_NOTIFICATIONSETTINGS_H
#define CORE_NOTIFICATIONSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/notification.h>
#include <QSet>

class QCheckBox;
class QTableWidget;

namespace Core {

typedef QList<QSet<QByteArray> > EnabledNotificationTypes;

QString notificationTypeName(int type);

class NotificationSettings : public qutim_sdk_0_3::SettingsWidget
{
    Q_OBJECT
public:
	explicit NotificationSettings(QWidget *parent = 0);
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
	static EnabledNotificationTypes enabledTypes();
signals:
	void enabledTypesChanged(const EnabledNotificationTypes &flagsList);
private slots:
	void onNotificationTypeChanged();
private:
	void updateTypesList();
	typedef QMap<QByteArray, QCheckBox*> BoxMap;
	BoxMap m_boxMap;
	QTableWidget *m_typesWidget;
	EnabledNotificationTypes m_enabledTypesList;
	QCheckBox *m_notificationInActiveChatBox;
	QCheckBox *m_conferenceMessagesBox; // "Ignore conference messages that do not contain my name"
	int m_currentRow;
};

} // namespace Core

#endif // CORE_NOTIFICATIONSETTINGS_H

