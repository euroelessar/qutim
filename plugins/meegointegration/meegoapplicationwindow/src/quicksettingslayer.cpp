/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "quicksettingslayer.h"
#include "quicksettingsmodel.h"
#include <qutim/menucontroller.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

QuickSettingsLayer::QuickSettingsLayer()
{
	m_models[0] = new QuickSettingsModel(this);
}

void QuickSettingsLayer::show(const qutim_sdk_0_3::SettingsItemList &settings, QObject *controller)
{
	if (m_models.contains(controller)) {
		update(settings, controller);
	} else {
		m_items[controller] = settings;
		QuickSettingsModel *model = new QuickSettingsModel(controller);
		m_models[controller] = model;
		model->setItems(settings);
		emit settingsRequested(model);
	}
}

void QuickSettingsLayer::close(QObject *controller)
{
	Q_UNUSED(controller);
//	m_items.remove(controller);
}

void QuickSettingsLayer::update(const qutim_sdk_0_3::SettingsItemList &settings, QObject *controller)
{
	Q_ASSERT(m_models.contains(controller));
	m_items[controller] = settings;
	m_models[controller]->setItems(settings);
}

QObject *QuickSettingsLayer::model()
{
	return m_models[0];
}

void QuickSettingsLayer::show(QObject *object)
{
	if (MenuController *controller = qobject_cast<MenuController*>(object))
		SettingsLayer::show(controller);
	else
		Settings::showWidget();
}
}
