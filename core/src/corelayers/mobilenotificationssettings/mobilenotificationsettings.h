/****************************************************************************
 *  mobilenotificationsettings.h
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

#ifndef CORE_MOBILENOTIFICATIONSETTINGS_H
#define CORE_MOBILENOTIFICATIONSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/notification.h>
#include <QSet>
#include <QTreeWidget>

class QCheckBox;

namespace Core {

QString notificationTypeName(int type);

class NotificationTreeItem : public QTreeWidgetItem
{
public:
	NotificationTreeItem(QTreeWidget *widget, const QIcon &icon, const QString &text);
	NotificationTreeItem(const QIcon &icon, const QString &text);
	NotificationTreeItem(const QString &text);
	virtual void setData(int column, int role, const QVariant &value);
protected:
	void setData(int column, int role, const QVariant &value, bool checkState);
};

class MobileNotificationSettings : public qutim_sdk_0_3::SettingsWidget
{
    Q_OBJECT
public:
	enum
	{
		BackendTypeRole = Qt::UserRole,
		NotificationTypeRole = Qt::UserRole + 1
	};

	explicit MobileNotificationSettings(QWidget *parent = 0);
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private slots:
	void onItemChanged(QTreeWidgetItem *item, int column);
private:
	struct Backend
	{
		qutim_sdk_0_3::NotificationBackend *backend;
		QTreeWidgetItem *item;
	};

	QTreeWidget *m_typesWidget;
	QList<Backend> m_backends;
	QCheckBox *m_notificationInActiveChatBox;
	QCheckBox *m_conferenceMessagesBox; // "Ignore conference messages that do not contain my name"

};

} // namespace Core

#endif // CORE_MOBILENOTIFICATIONSETTINGS_H
