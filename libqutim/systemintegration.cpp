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
#include <QStringList>

namespace qutim_sdk_0_3
{
	Q_GLOBAL_STATIC(SystemIntegrationHook, staticHook)
	
	inline bool integrationLessThan(SystemIntegration *a, SystemIntegration *b)
	{ return a->priority() > b->priority(); }
	
	SystemIntegrationHook::SystemIntegrationHook()
	{
		foreach(const ObjectGenerator *gen, ObjectGenerator::module<SystemIntegration>()) {
			SystemIntegration *integration = gen->generate<SystemIntegration>();
			if (!integration->isAvailable())
				delete integration;
			else
				m_integrations.append(integration);
		}
		qSort(m_integrations.begin(), m_integrations.end(), integrationLessThan);
	}

	SystemIntegrationHook::~SystemIntegrationHook()
	{
	}
	
	void SystemIntegrationHook::init()
	{
	}

	bool SystemIntegrationHook::isAvailable() const
	{
		return true;
	}
	
	int SystemIntegrationHook::priority()
	{
		return 0;
	}
	
	QVariant SystemIntegrationHook::value(Attribute attr, const QVariant &data) const
	{
		QVariant result;
		for (int i = 0; i < m_integrations.size(); i++) {
			result = m_integrations.at(i)->value(attr, data);
			if (result.isNull() || !result.isValid())
				continue;
		}
		return result;
	}
	
	SystemIntegration::SystemIntegration()
	{
	}
	
	SystemIntegration::~SystemIntegration()
	{
	}
	
	SystemIntegration *SystemIntegration::instance()
	{
		return staticHook();
	}
	
	void SystemIntegration::virtual_hook(int type, void *data)
	{
		Q_UNUSED(type);
		Q_UNUSED(data);
	}
}
