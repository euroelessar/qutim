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

