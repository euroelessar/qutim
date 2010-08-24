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

#ifndef SYSTEMINTEGRATION_H
#define SYSTEMINTEGRATION_H

#include "libqutim_global.h"
#include <QVariant>

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT SystemIntegration
	{
	public:
		enum Attribute { 
			UserLogin,
			UserName,
			CurrentLanguage, // not the same as locale
			ExtensionsForMimeType, // /etc/mime.types
			TimeZone,
			TimeZoneName,
			TimeZoneShortName
		};
		enum Priority
		{
			BaseSystem = 0, // Unix
			WindowManager = 127, // X11
			DesktopEnvironment = 255 // KDE
		};

		SystemIntegration();
		virtual ~SystemIntegration();
		
		static SystemIntegration *instance();
		
		virtual void init() = 0;
		virtual bool isAvailable() const = 0;
		
		virtual int priority() = 0;
		
		virtual QVariant value(Attribute attr, const QVariant &data = QVariant()) const = 0;
	//	virtual QVariant process(Operation act, const QVariantList &args = QVariantList()) const = 0;
	protected:
		virtual void virtual_hook(int, void *);
	};
}

#endif // SYSTEMINTEGRATION_H

Q_DECLARE_INTERFACE(qutim_sdk_0_3::SystemIntegration, "org.qutim.SystemIntegration")
