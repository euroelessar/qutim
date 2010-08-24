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

#ifndef SYSTEMINTEGRATION_P_H
#define SYSTEMINTEGRATION_P_H

#include "systemintegration.h"

namespace qutim_sdk_0_3
{
	class SystemIntegrationHook : public SystemIntegration
	{
	public:
		SystemIntegrationHook();
		virtual ~SystemIntegrationHook();
		
		virtual void init();
		virtual bool isAvailable() const;
		
		virtual int priority();
		
		virtual QVariant value(Attribute attr, const QVariant &data = QVariant()) const;
	//	virtual QVariant process(Operation act, const QVariantList &args = QVariantList()) const;
	private:
		QList<SystemIntegration*> m_integrations;
	};
}
#endif // SYSTEMINTEGRATION_P_H
