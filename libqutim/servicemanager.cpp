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
#include "metaobjectbuilder.h"
#include <QHash>
#include <debug.h>

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

ServiceManager *ServiceManager::instance()
{
	static ServiceManager manager;
	return &manager;
}

ExtensionInfoList ServiceManager::listImplementations(const QByteArray &name)
{
	return moduleManagerPrivate()->allServices.values(name);
}

static bool checkSwitchSupport(const QMetaObject *meta)
{
	const char *switchSupport = MetaObjectBuilder::info(meta, "RuntimeSwitch");
	return switchSupport &&
			(QString::compare(switchSupport, "yes", Qt::CaseInsensitive) == 0 ||
			qstrcmp(switchSupport, "1") == 0);
}

bool ServiceManager::setImplementation(const QByteArray &name, const ExtensionInfo &info)
{
	ModuleManagerPrivate *p = moduleManagerPrivate();

	const QMetaObject *newMeta = info.generator()->metaObject();
	if (!info.generator() || qstrcmp(name.constData(), MetaObjectBuilder::info(newMeta, "Service")) != 0) {
		// TODO: disable the service
		return false;
	}

	QObject *old = p->services.value(name);
	const QMetaObject *oldMeta = old->metaObject();
	if (oldMeta == newMeta)
		return true;

	Config cfg = Config().group("services/list");
	cfg.setValue(name, newMeta->className());

	if (!checkSwitchSupport(oldMeta) || !checkSwitchSupport(newMeta))
		return false;

	QObject *now = info.generator()->generate<QObject>();
	debug() << now->metaObject()->className();
	p->services.insert(name, now);
	int pos = p->serviceOrder.indexOf(old);
	Q_ASSERT(pos != -1);
	p->serviceOrder.replace(pos, now);
	emit instance()->serviceChanged(name, now, old);
	old->deleteLater();
	return true;
}

} // namespace qutim_sdk_0_3
