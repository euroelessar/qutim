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

#include "systemintegration_p.h"
#include "objectgenerator.h"
#include "settingslayer.h"
#include <limits>
#include <QStringList>
#include <QWidget>
#include <QDialog>
#include <QApplication>
#include <QAbstractSocket>

namespace qutim_sdk_0_3
{
struct IntegrationData
{
	QList<SystemIntegration*> integrations;
	
	IntegrationData();

	template <typename T>
	SystemIntegration *find(T type);
};

Q_GLOBAL_STATIC(IntegrationData, integrationData)

IntegrationData::IntegrationData()
{
	integrations.append(new DefaultSystemIntegration);
	foreach(const ObjectGenerator *gen, ObjectGenerator::module<SystemIntegration>()) {
		SystemIntegration *integration = gen->generate<SystemIntegration>();
		if (!integration->isAvailable()) {
			delete integration;
		} else {
			integration->init();
			integrations.append(integration);
		}
	}
    qSort(integrations.begin(), integrations.end(),
          [] (SystemIntegration *a, SystemIntegration *b) {
        return a->priority() > b->priority();
    });
}

template <typename T>
Q_INLINE_TEMPLATE SystemIntegration *IntegrationData::find(T type)
{
	for (int i = 0; i < integrations.size(); i++) {
		if (integrations[i]->canHandle(type))
			return integrations[i];
	}
	return 0;
}

DefaultSystemIntegration::DefaultSystemIntegration()
{
}

DefaultSystemIntegration::~DefaultSystemIntegration()
{
}

void DefaultSystemIntegration::init()
{
}

bool DefaultSystemIntegration::isAvailable() const
{
	return true;
}

int DefaultSystemIntegration::priority()
{
	return std::numeric_limits<int>::min();
}

QVariant DefaultSystemIntegration::doGetValue(Attribute attr, const QVariant &data) const
{
	Q_UNUSED(attr);
	Q_UNUSED(data);
	return QVariant();
}

QVariant DefaultSystemIntegration::doProcess(Operation act, const QVariant &data) const
{
	switch (act) {
	case ShowWidget: {
		QWidget *widget = data.value<QWidget*>();
#if	defined(QUTIM_MOBILE_UI)
		widget->showMaximized();
#else
		widget->show();
#endif
		break;
	}
	case OpenDialog: {
		QWidget *widget = data.value<QWidget*>();
		if (QDialog *dialog = qobject_cast<QDialog*>(widget))
			dialog->open();
		else
			SystemIntegration::show(widget);
	}
	default:
		break;
	}
	return QVariant();
}

bool DefaultSystemIntegration::canHandle(Attribute attribute) const
{
	Q_UNUSED(attribute);
	return false;
}

bool DefaultSystemIntegration::canHandle(Operation operation) const
{
	return operation == ShowWidget || operation == OpenDialog;
}

SystemIntegration::SystemIntegration()
{
}

SystemIntegration::~SystemIntegration()
{
}

QVariant SystemIntegration::value(Attribute attr, const QVariant &data)
{
	if (SystemIntegration *integration = integrationData()->find(attr))
		return integration->doGetValue(attr, data);
	return QVariant();
}

QVariant SystemIntegration::process(Operation act, const QVariant &data)
{
	if (SystemIntegration *integration = integrationData()->find(act))
		return integration->doProcess(act, data);
	return QVariant();
}

void SystemIntegration::virtual_hook(int type, void *data)
{
	Q_UNUSED(type);
	Q_UNUSED(data);
}

void SystemIntegration::show(QWidget *widget)
{
	process(ShowWidget, QVariant::fromValue(widget));
}

void SystemIntegration::open(QDialog *dialog)
{
	process(OpenDialog, QVariant::fromValue<QWidget*>(dialog));
}

void SystemIntegration::keepAlive(QAbstractSocket *socket)
{
	process(KeepAliveSocket, QVariant::fromValue<QObject*>(socket));
}

ObjectGenerator *SystemIntegration::settingsGenerator(SettingsItem *item)
{
	QVariant result = process(GetSettingsGenerator, QVariant::fromValue(item));
	return result.value<ObjectGenerator*>();
}

}
