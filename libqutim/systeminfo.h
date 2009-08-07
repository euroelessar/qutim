/*****************************************************************************
	System Info

	Copyright (c) 2007-2008 by Remko Tronçon
				  2008-2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include "libqutim_global.h"
#include <QPointer>

namespace qutim_sdk_0_3
{
	struct SystemInfoPrivate;

	class LIBQUTIM_EXPORT SystemInfo : public QObject
	{
		Q_OBJECT
	public:
		static const SystemInfo &instance();
		QString osFull() const;
		QString osName() const;
		QString osVersion() const;
		QString timezone() const;
		int timezoneOffset() const;
	private:
		SystemInfo();
		virtual ~SystemInfo();
		static QPointer<SystemInfo> self;
		SystemInfoPrivate *p;
	};
}

#endif // SYSTEMINFO_H
