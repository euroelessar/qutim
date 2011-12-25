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

#ifndef SERVICEMANAGER_P_H
#define SERVICEMANAGER_P_H

#include "servicemanager.h"
#include <QHash>

namespace qutim_sdk_0_3
{

typedef QHash<QByteArray, ExtensionInfoList> ServiceHash;
typedef QHash<QByteArray, ExtensionInfo> CheckedServiceHash;

class ServiceManagerPrivate
{
	Q_DECLARE_PUBLIC(ServiceManager)
public:
	ServiceManagerPrivate(ServiceManager *q) : q_ptr(q), inited(false) {}
	ServiceManager *q_ptr;
	ServiceHash infoHash;
	CheckedServiceHash checked;
	QHash<QByteArray, QSharedPointer<ServicePointerData> > hash;
	QList<ServicePointerData*> initializationOrder;
	bool inited;
	
	void init();
	void init(const QByteArray &service, const ExtensionInfo &info, QSet<QByteArray> &used);
	void deinit();
	ServicePointerData *data(const QByteArray &name);
	static ServiceManagerPrivate *get(ServiceManager *q) { return q->d_func(); }
};

}

#endif // SERVICEMANAGER_P_H

