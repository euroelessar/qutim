/****************************************************************************
 *  servicemanager.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include "servicemanager.h"
#include "modulemanager_p.h"
#include <QHash>

namespace qutim_sdk_0_3 {

ServiceManager::ServiceManager()
{
}

ServiceManager::~ServiceManager()
{
}

QObject *ServiceManager::getByName(const QByteArray &name)
{
	return services().value(name);
}

QList<QByteArray> ServiceManager::names()
{
	return services().keys();
}

} // namespace qutim_sdk_0_3
