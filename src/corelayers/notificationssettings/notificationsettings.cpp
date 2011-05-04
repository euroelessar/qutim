/****************************************************************************
 *  notificationsettings.cpp
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

#include "notificationsettings.h"

#include <QFormLayout>
#include <QMetaEnum>
#include <QCheckBox>
#include <QLabel>

#include <qutim/notification.h>
#include <qutim/config.h>

namespace Core {

using namespace qutim_sdk_0_3;

NotificationSettings::NotificationSettings(QWidget *parent) :
    qutim_sdk_0_3::SettingsWidget(parent)
{
	QFormLayout *layout =  new QFormLayout(this);
	layout->setContentsMargins(0, 6, 6, 6);
	QLabel *label = new QLabel(tr("List of available notifications:"), this);
	layout->addWidget(label);
}

void NotificationSettings::addNotify(qutim_sdk_0_3::Notification::Type type, bool set)
{
	LocalizedString str = Notification::typeString(type);
	QCheckBox *box = new QCheckBox(str.toString(), this);
	box->setChecked(set);
	m_boxMap.insert(type, box);
	layout()->addWidget(box);
	lookForWidgetState(box);
}

void NotificationSettings::loadImpl()
{
	qDeleteAll(m_boxMap);
	m_boxMap.clear();
	Config cfg;
	cfg.beginGroup("notification");
	int flags = cfg.value("flags", -1);
	QMetaObject meta = Notification::staticMetaObject;
	for (int i = 0; i < meta.enumeratorCount(); ++i) {
		QMetaEnum e = meta.enumerator(i);
		if (e.name() == QLatin1String("Type"))
			for (int j = 0;j != e.keyCount(); j++)
				addNotify(static_cast<Notification::Type>(e.value(j)), flags & e.value(j));
	}
}

void NotificationSettings::cancelImpl()
{
	loadImpl();
}

void NotificationSettings::saveImpl()
{
	QMap<qutim_sdk_0_3::Notification::Type, QCheckBox*>::const_iterator it = m_boxMap.constBegin();
	int flags = 0;
	for (; it != m_boxMap.constEnd(); it++)
		if (it.value()->isChecked())
			flags |= it.key();
	Config cfg;
	cfg.beginGroup("notification");
	cfg.setValue("flags", flags);	
}

} // namespace Core
