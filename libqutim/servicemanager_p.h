/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
	QHash<QByteArray, QObject*> hash;
	QList<QObject*> initializationOrder;
	bool inited;
	
	void init();
	void init(const QByteArray &service, const ExtensionInfo &info, QSet<QByteArray> &used);
	void deinit();
	static ServiceManagerPrivate *get(ServiceManager *q) { return q->d_func(); }
};

}

#endif // SERVICEMANAGER_P_H
