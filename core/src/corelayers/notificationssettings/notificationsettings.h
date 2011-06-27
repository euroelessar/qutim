/****************************************************************************
 *  notificationsettings.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef CORE_NOTIFICATIONSETTINGS_H
#define CORE_NOTIFICATIONSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/notification.h>
#include <QSet>

class QCheckBox;
class QTableWidget;

namespace Core {

typedef QList<QSet<QByteArray> > EnabledNotificationTypes;

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
