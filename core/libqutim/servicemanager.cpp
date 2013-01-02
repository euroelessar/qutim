/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "servicemanager_p.h"
#include "modulemanager_p.h"
#include "metaobjectbuilder.h"
#include <QHash>
#include <QMetaClassInfo>

namespace qutim_sdk_0_3
{

ServicePointerData::ServicePointerData() : object(0)
{
}

void ServiceManagerPrivate::init()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("services/list"));
	const ExtensionInfoList extensions = extensionList();
	QSet<QByteArray> used;
	for (int i = 0; i < extensions.size(); ++i) {
		const ExtensionInfo &info = extensions.at(i);
		const QMetaObject *meta = info.generator()->metaObject();
		const char *service = MetaObjectBuilder::info(meta, "Service");
		if (qstrlen(service)) {
			QByteArray serviceName = service;
			infoHash[serviceName] << info;
			QString checkedImpl = cfg.value(QLatin1String(service), QString());
			if (checkedImpl == QLatin1String("none"))
				continue;
			if (checkedImpl == QLatin1String(meta->className())) {
				checked.insert(serviceName, info);
			} else if (!checked.contains(serviceName)) {
				checked.insert(serviceName, info);
			}
		}
	}
	CheckedServiceHash::Iterator it = checked.begin();
	for (; it != checked.end(); ++it) { 
		if (!it.value().generator())
			continue;
		init(it.key(), it.value(), used);
		const QMetaObject *meta = it.value().generator()->metaObject();
		cfg.setValue(QLatin1String(it.key()), QLatin1String(meta->className()));
	}
	inited = true;
	emit q_func()->initedChanged(inited);
}

void ServiceManagerPrivate::init(const QByteArray &service, const ExtensionInfo &info, QSet<QByteArray> &used)
{
	if (used.contains(service) || !info.generator())
		return;
	used << service;
	const QMetaObject *meta = info.generator()->metaObject();
	QMetaClassInfo classInfo;
	for (int i = 0; i < meta->classInfoCount(); ++i) {
		classInfo = meta->classInfo(i);
		if (!qstrcmp(classInfo.name(), "Uses")) {
			QByteArray id = classInfo.value();
			init(id, checked.value(id), used);
		}
	}
	QObject *object = info.generator()->generate();
	initializationOrder << data(service);
	initializationOrder.last()->object = object;
}

void ServiceManagerPrivate::deinit()
{
	for (int i = initializationOrder.size() - 1; i >= 0; --i) {
		QObject * &object = initializationOrder.at(i)->object;
		delete object;
		object = 0;
	}
	initializationOrder.clear();
	hash.clear();
}

ServicePointerData *ServiceManagerPrivate::data(const QByteArray &name)
{
	QSharedPointer<ServicePointerData> &d = hash[name];
	if (!d) {
		d = QSharedPointer<ServicePointerData>::create();
		d->name = name;
	}
	return d.data();
}

ServiceManager::ServiceManager() : d_ptr(new ServiceManagerPrivate(this))
{
}

ServiceManager::~ServiceManager()
{
}

ServiceManager *ServiceManager::instance()
{
	static ServiceManager manager;
	return &manager;
}

bool ServiceManager::isInited()
{
	return ServiceManagerPrivate::get(instance())->inited;
}

QObject *ServiceManager::getByName(const QByteArray &name)
{
	return ServiceManagerPrivate::get(instance())->data(name)->object;
}

QList<QByteArray> ServiceManager::names()
{
	return ServiceManagerPrivate::get(instance())->infoHash.keys();
}

ExtensionInfoList ServiceManager::listImplementations(const QByteArray &name)
{
	return ServiceManagerPrivate::get(instance())->infoHash.value(name);
}

// TODO: Try to investigate
//static bool checkSwitchSupport(const QMetaObject *meta)
//{
//	const char *switchSupport = MetaObjectBuilder::info(meta, "RuntimeSwitch");
//	return switchSupport &&
//			(QString::compare(switchSupport, "yes", Qt::CaseInsensitive) == 0 ||
//			qstrcmp(switchSupport, "1") == 0);
//}

bool ServiceManager::setImplementation(const QByteArray &name, const ExtensionInfo &info)
{
	ServiceManagerPrivate *d = ServiceManagerPrivate::get(instance());
	ExtensionInfo currentInfo = d->checked.value(name);
	if (currentInfo.generator() == info.generator())
		return true;
	Q_ASSERT(!info.generator() || MetaObjectBuilder::info(info.generator()->metaObject(), "Service") == name);
	Config cfg;
	cfg.beginGroup(QLatin1String("services/list"));
	if (!info.generator()) {
		cfg.setValue(QLatin1String(name), QLatin1String("none"));
	} else {
		cfg.setValue(QLatin1String(name), QLatin1String(info.generator()->metaObject()->className()));
	}
	d->checked.insert(name, info);
	ServicePointerData *data = d->data(name);
	QObject * &object = data->object;
	QObject *oldObject = info.generator() ? info.generator()->generate() : 0;
	qSwap(oldObject, object);
	if (!oldObject && object) {
		d->initializationOrder << data;
	} else if (oldObject && !object) {
		int index = d->initializationOrder.indexOf(data);
		Q_ASSERT(index != -1);
		d->initializationOrder.removeAt(index);
	}
	emit instance()->serviceChanged(name, object, oldObject);
	emit instance()->serviceChanged(object, oldObject);
	if (oldObject)
		oldObject->deleteLater();
	return true;
}

ServicePointerData::Ptr ServiceManager::getData(const QMetaObject *meta)
{
	return getData(MetaObjectBuilder::info(meta, "Service"));
}

ServicePointerData::Ptr ServiceManager::getData(const QByteArray &name)
{
	ServiceManagerPrivate *d = ServiceManagerPrivate::get(instance());
	QSharedPointer<ServicePointerData> &data = d->hash[name];
	if (!data) {
		data = QSharedPointer<ServicePointerData>::create();
		data->name = name;
	}
	return data.toWeakRef();
}

} // namespace qutim_sdk_0_3

