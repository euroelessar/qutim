/****************************************************************************
 *  serviceicons.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef SERVICEICONS_H
#define SERVICEICONS_H

#include "libqutim/icon.h"
#include <QtCore/QHash>

namespace Adium
{
	class ServiceIcons : public QObject, public qutim_sdk_0_3::IconWrapper
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::IconWrapper)
	public:
		ServiceIcons();
		virtual ~ServiceIcons();
		virtual QIcon getIcon(const QString &name);
	private:
		QHash<QString, QIcon> m_icons;
	};
}

#endif // SERVICEICONS_H
