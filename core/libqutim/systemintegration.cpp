/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "systemintegration_p.h"
#include "objectgenerator.h"
#include <limits>
#include <QStringList>
#include <QWidget>
#include <QApplication>

namespace qutim_sdk_0_3
{
struct IntegrationData
{
	QList<SystemIntegration*> integrations;
	
	void init();
	template <typename T>
	SystemIntegration *find(T type);
};

Q_GLOBAL_STATIC_WITH_INITIALIZER(IntegrationData, integrationData, x->init())

inline bool integrationLessThan(SystemIntegration *a, SystemIntegration *b)
{
	return a->priority() > b->priority();
}

void IntegrationData::init()
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
	qSort(integrations.begin(), integrations.end(), integrationLessThan);
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
	return operation == ShowWidget;
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
	process(ShowWidget, qVariantFromValue(widget));
}

}
