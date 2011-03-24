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
