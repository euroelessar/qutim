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

#include "mobilenotificationsettings.h"

#include <QMetaEnum>
#include <QCheckBox>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHeaderView>

#include <qutim/notification.h>
#include <qutim/config.h>

namespace Core {

using namespace qutim_sdk_0_3;

QString notificationTypeName(int type)
{
	static QStringList names;
	if (names.isEmpty()) {
		for (int i = 0; i <= Notification::LastType; ++i) {
			const QMetaObject &meta = Notification::staticMetaObject;
			QMetaEnum e = meta.enumerator(meta.indexOfEnumerator("Type"));
			names << e.key(i);
		}
	}
	return names.value(type);
}

NotificationTreeItem::NotificationTreeItem(QTreeWidget *widget, const QIcon &icon, const QString &text) :
	QTreeWidgetItem(widget, QStringList() << text)
{
	setData(0, Qt::DecorationRole, icon);
}

NotificationTreeItem::NotificationTreeItem(const QIcon &icon, const QString &text) :
	QTreeWidgetItem(QStringList() << text)
{
	setData(0, Qt::DecorationRole, icon);
}

NotificationTreeItem::NotificationTreeItem(const QString &text) :
	QTreeWidgetItem(QStringList() << text)
{
}

void NotificationTreeItem::setData(int column, int role, const QVariant &value)
{
	setData(column, role, value, true);
}

void NotificationTreeItem::setData(int column, int role, const QVariant &value, bool checkState)
{
	 if (checkState && role == Qt::CheckStateRole) {
		 Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());

		 if (state != Qt::PartiallyChecked) {
			 for (int row = 0; row != childCount(); ++row)
				 static_cast<NotificationTreeItem*>(child(row))->setData(0, role, state, false);
		 }

		 QTreeWidgetItem *parent = this->parent();
		 if (parent) {
			 QVariant parentState = value;
			 for (int i = 0, n = parent->childCount(); i < n; ++i) {
				 QTreeWidgetItem *child = parent->child(i);
				 if (child == this)
					 continue;

				 if (parentState != child->data(0, role)) {
					 parentState = Qt::PartiallyChecked;
					 break;
				 }
			 }
			 static_cast<NotificationTreeItem*>(parent)->setData(0, role, parentState, false);
		 }
	 }

	 QTreeWidgetItem::setData(column, role, value);
}


MobileNotificationSettings::MobileNotificationSettings(QWidget *parent) :
	qutim_sdk_0_3::SettingsWidget(parent)
{
	QVBoxLayout *layout =  new QVBoxLayout(this);
	layout->setContentsMargins(0, 6, 6, 6);

	m_typesWidget = new QTreeWidget(this);
	m_typesWidget->setAlternatingRowColors(true);
	m_typesWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_typesWidget->header()->hide();

	foreach (NotificationBackend *backend, NotificationBackend::all()) {
		QString backendDesc = backend->description();
		if (backendDesc.isEmpty())
			 backendDesc = LocalizedString(backend->backendType());

		QTreeWidgetItem *backendItem = new NotificationTreeItem(m_typesWidget, QIcon(), backendDesc);
		backendItem->setData(0, BackendTypeRole, backend->backendType());
		backendItem->setCheckState(0, Qt::PartiallyChecked);

		Backend tmp = { backend, backendItem };
		m_backends.push_back(tmp);

		for (int i = 0; i <= Notification::LastType; ++i) {
			Notification::Type type = static_cast<Notification::Type>(i);
			QString typeDesc = Notification::typeString(type);
			QTreeWidgetItem *item = new NotificationTreeItem(typeDesc);
			item->setData(0, NotificationTypeRole, i);
			backendItem->addChild(item);
		}
	}

	//m_typesWidget->expandAll();
	layout->addWidget(m_typesWidget);
	connect(m_typesWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
			SLOT(onItemChanged(QTreeWidgetItem*,int)));

	m_notificationInActiveChatBox = new QCheckBox(tr("Disable notifications when chat is active"), this);
	layout->addWidget(m_notificationInActiveChatBox);
	lookForWidgetState(m_notificationInActiveChatBox);

	m_conferenceMessagesBox = new QCheckBox(tr("Ignore conference messages that do not contain my name"), this);
	layout->addWidget(m_conferenceMessagesBox);
	lookForWidgetState(m_conferenceMessagesBox);

#ifdef Q_WS_MAEMO_5
	m_notificationInActiveChatBox->setMaximumHeight(40);
	m_conferenceMessagesBox->setMaximumHeight(40);
#endif

}

void MobileNotificationSettings::loadImpl()
{
	Config cfg;
	cfg.beginGroup("notification");
	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		cfg.beginGroup(notificationTypeName(type));
		foreach (const Backend &b, m_backends) {
			QByteArray backendType = b.backend->backendType();
			QTreeWidgetItem *item = b.item->child(i);
			Q_ASSERT(item);
			item->setCheckState(0, cfg.value(backendType, false) ? Qt::Checked : Qt::Unchecked);
		}
		cfg.endGroup();
	}

	// Load additional settings
	m_conferenceMessagesBox->setChecked(cfg.value("ignoreConfMsgsWithoutUserNick", true));
	cfg.endGroup();
	cfg = Config("appearance").group("chat");
	m_notificationInActiveChatBox->setChecked(!cfg.value("notificationsInActiveChat", true));
}

void MobileNotificationSettings::cancelImpl()
{
	loadImpl();
}

void MobileNotificationSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup("notification");
	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		cfg.beginGroup(notificationTypeName(type));
		foreach (const Backend &b, m_backends) {
			QByteArray backendType = b.backend->backendType();
			QTreeWidgetItem *item = b.item->child(i);
			Q_ASSERT(item);
			cfg.setValue(backendType, item->checkState(0) == Qt::Checked);
		}
		cfg.endGroup();
	}

	cfg.setValue("ignoreConfMsgsWithoutUserNick", m_conferenceMessagesBox->isChecked());

	cfg.endGroup();

	cfg = Config("appearance").group("chat");
	cfg.setValue("notificationsInActiveChat", !m_notificationInActiveChatBox->isChecked());
}

void MobileNotificationSettings::onItemChanged(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(item);
	Q_UNUSED(column);
	setModified(true);
}

} // namespace Core

