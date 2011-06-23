/****************************************************************************
 *  notificationsettings.cpp
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

#include "notificationsettings.h"

#include <QMetaEnum>
#include <QCheckBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>

#include <qutim/notification.h>
#include <qutim/config.h>

namespace Core {

using namespace qutim_sdk_0_3;

static QString notificationTypeName(Notification::Type type)
{
	QStringList names;
	if (names.isEmpty()) {
		for (int i = 0; i <= Notification::LastType; ++i) {
			QMetaObject meta = Notification::staticMetaObject;
			QMetaEnum e = meta.enumerator(meta.indexOfEnumerator("Type"));
			names << e.key(i);
		}
	}
	return names.value(type);
}

NotificationSettings::NotificationSettings(QWidget *parent) :
	qutim_sdk_0_3::SettingsWidget(parent), m_currentRow(-1)
{
	QVBoxLayout *layout =  new QVBoxLayout(this);
	layout->setContentsMargins(0, 6, 6, 6);

	m_typesWidget = new QTableWidget(Notification::LastType+1, 1, this);
	m_typesWidget->setShowGrid(false);
	m_typesWidget->setAlternatingRowColors(true);
	m_typesWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_typesWidget->horizontalHeader()->setStretchLastSection(true);
	m_typesWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	m_typesWidget->horizontalHeader()->hide();
	m_typesWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	m_typesWidget->verticalHeader()->hide();

	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		QTableWidgetItem *item = new QTableWidgetItem(Notification::typeString(type));
		m_typesWidget->setItem(i, 0, item);

		// TODO: add description column
	}

	m_typesWidget->setCurrentCell(0, 0);
	layout->addWidget(m_typesWidget);
	connect(m_typesWidget,
			SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),
			SLOT(onNotificationTypeChanged()));

	foreach (NotificationBackend *backend, NotificationBackend::all()) {
		QString desc = backend->description();
		if (desc.isEmpty())
			 desc = LocalizedString(backend->backendType());
		QCheckBox *box = new QCheckBox(desc, this);
		m_boxMap.insert(backend->backendType(), box);
		layout->addWidget(box);
		lookForWidgetState(box);
	}

	//layout->addSpacerItem(new QSpacerItem(0, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
}

void NotificationSettings::onNotificationTypeChanged()
{
	// Store current values
	updateTypesList();

	// Update checkboxes
	m_currentRow = m_typesWidget->currentRow();
	Q_ASSERT(m_currentRow >= 0 && m_currentRow <= Notification::LastType);

	Notification::Type type = static_cast<Notification::Type>(m_currentRow);
	BoxMap::Iterator itr = m_boxMap.begin();
	BoxMap::Iterator end = m_boxMap.end();
	for (; itr != end; ++itr) {
		bool isSet = m_enabledTypesList.value(type).contains(itr.key());
		// If we do not block checkbox's signals, modifiedChanged() will be emitted
		(*itr)->blockSignals(true);
		(*itr)->setChecked(isSet);
		(*itr)->blockSignals(false);
	}
}

void NotificationSettings::updateTypesList()
{
	if (m_currentRow >= 0 && m_currentRow <= Notification::LastType) {
		QSet<QByteArray> backendTypes;
		BoxMap::Iterator itr = m_boxMap.begin();
		BoxMap::Iterator end = m_boxMap.end();
		for (; itr != end; ++itr) {
			if ((*itr)->isChecked())
				backendTypes << itr.key();
		}

		m_enabledTypesList[m_currentRow] = backendTypes;
	}
}

void NotificationSettings::loadImpl()
{
	m_enabledTypesList = enabledTypes();
	onNotificationTypeChanged();
}

void NotificationSettings::cancelImpl()
{
	loadImpl();
}

void NotificationSettings::saveImpl()
{
	updateTypesList();
	Config cfg;
	cfg.beginGroup("notification");

	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		QSet<QByteArray> enabledBackends = m_enabledTypesList.at(i);

		cfg.beginGroup(notificationTypeName(type));
		foreach (NotificationBackend *backend, NotificationBackend::all()) {
			QByteArray backendType = backend->backendType();
			cfg.setValue(backendType, enabledBackends.contains(backendType));
		}
		cfg.endGroup();
	}

	cfg.endGroup();
	emit enabledTypesChanged(m_enabledTypesList);
}

EnabledNotificationTypes NotificationSettings::enabledTypes()
{
	EnabledNotificationTypes enabledTypesList;
	Config cfg;
	cfg.beginGroup("notification");

	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		QSet<QByteArray> backendTypes;
		cfg.beginGroup(notificationTypeName(type));
		foreach (NotificationBackend *backend, NotificationBackend::all()) {
			QByteArray backendType = backend->backendType();
			if (cfg.value(backendType, true))
				backendTypes << backendType;
		}
		cfg.endGroup();
		enabledTypesList << backendTypes;
	}

	cfg.endGroup();
	return enabledTypesList;
}

} // namespace Core
