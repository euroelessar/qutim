#include "quickservice.h"
#include <qutim/servicemanager.h>

namespace qutim_sdk_0_3 {

QuickService::QuickService(QObject *parent) :
    QObject(parent)
{
    connect(ServiceManager::instance(), &ServiceManager::serviceChanged, this, &QuickService::onServiceChanged);
}

QuickService::~QuickService()
{
}

QString QuickService::name() const
{
    return m_name;
}

void QuickService::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        m_utf8Name = name.toUtf8();

        m_object = ServiceManager::instance()->getByName(m_utf8Name);

        emit nameChanged(name);
    }
}

QObject *QuickService::object() const
{
    return m_object;
}

void QuickService::onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject)
{
    Q_UNUSED(oldObject);

    if (name == m_utf8Name) {
        m_object = newObject;
        emit objectChanged(m_object);
    }
}

} // namespace qutim_sdk_0_3
