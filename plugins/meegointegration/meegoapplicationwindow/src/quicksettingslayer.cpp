/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "applicationwindow.h"
#include <qutim/menucontroller.h>
#include <qutim/servicemanager.h>
#include <qutim/thememanager.h>
#include <QDeclarativeItem>
#include <QDeclarativeComponent>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

QuickSettingsLayer::QuickSettingsLayer()
{
	m_models[0] = new QuickSettingsModel(this);
}

void QuickSettingsLayer::show(const qutim_sdk_0_3::SettingsItemList &settings, QObject *controller)
{
	QuickSettingsModel * &model = m_models[controller];
	if (model) {
		update(settings, controller);
	} else {
		m_items[controller] = settings;
		model = new QuickSettingsModel(controller);
		model->setItems(settings, controller);
	}
	emit settingsRequested(model);
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
	m_models[controller]->setItems(settings, controller);
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

QuickGenerator::QuickGenerator(const QString &component) : m_component(component)
{
}

QuickGenerator::~QuickGenerator()
{
}

QObject *QuickGenerator::generateHelper() const
{
	ServicePointer<ApplicationWindow> app;
	QString filePath = ThemeManager::path(QLatin1String("declarative"),
	                                      QLatin1String("meego"));
	QUrl url = QUrl::fromLocalFile(filePath + QLatin1Char('/') + m_component);
	QDeclarativeComponent component(app->engine(), url);
	qDebug() << url << component.isLoading() << component.isError() << component.errorString();
	return component.create();
}

const QMetaObject *QuickGenerator::metaObject() const
{
	return &QDeclarativeItem::staticMetaObject;
}

QuickSettingsItem::QuickSettingsItem(const QString &component, Settings::Type type, const QIcon &icon, const LocalizedString &text)
	: SettingsItem(type, icon, text), m_component(component)
{
}

QuickSettingsItem::QuickSettingsItem(const QString &component, Settings::Type type, const LocalizedString &text)
	: SettingsItem(type, text), m_component(component)
{
}

QuickSettingsItem::~QuickSettingsItem()
{
}

const ObjectGenerator *QuickSettingsItem::generator() const
{
	return new QuickGenerator(m_component);
}
}

