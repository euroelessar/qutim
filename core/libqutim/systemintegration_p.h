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
	class DefaultSystemIntegration : public SystemIntegration
	{
	public:
		DefaultSystemIntegration();
		virtual ~DefaultSystemIntegration();
		
		virtual void init();
		virtual bool isAvailable() const;
		
		virtual int priority();
		
		
	protected:
		virtual QVariant doGetValue(Attribute attr, const QVariant &data) const;
		virtual QVariant doProcess(Operation act, const QVariant &data) const;
		virtual bool canHandle(Attribute attribute) const;
		virtual bool canHandle(Operation operation) const;
		
	private:
		QList<SystemIntegration*> m_integrations;
	};
}
#endif // SYSTEMINTEGRATION_P_H
