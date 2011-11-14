/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "servicemanagerwrapper.h"
#include <qutim/servicemanager.h>

namespace MeegoIntegration
{
ServiceManagerWrapper::ServiceManagerWrapper()
{
	qutim_sdk_0_3::ServiceManager *manager = qutim_sdk_0_3::ServiceManager::instance();
	foreach (const QByteArray &name, manager->names())
		onServiceChanged(name, manager->getByName(name));
}

void ServiceManagerWrapper::init()
{
	qmlRegisterType<ServiceManagerWrapper>("org.qutim", 0, 3, "ServiceManager");
}

void ServiceManagerWrapper::onServiceChanged(const QByteArray &rawName, QObject *object)
{
	Q_ASSERT(!rawName.isEmpty());
	QString name = QString::fromUtf8(rawName);
	Q_ASSERT(!name.isEmpty());
	name[0] = name[0].toLower();
	insert(name, qVariantFromValue(object));
}
}
