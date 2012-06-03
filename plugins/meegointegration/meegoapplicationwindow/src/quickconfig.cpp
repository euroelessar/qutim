#include "quickconfig.h"

using namespace qutim_sdk_0_3;

QuickConfig::QuickConfig(QObject *parent) :
    QObject(parent)
{
}

QVariant QuickConfig::value(const QString &key, const QVariant &def)
{
    return m_config.value(key, def);
}

void QuickConfig::setValue(const QString &key, const QVariant &value)
{
    m_config.setValue(key, value);
    m_config.sync();
}

QString QuickConfig::path() const
{
    return m_path;
}

void QuickConfig::setPath(const QString &path)
{
    if (m_path != path) {
        m_path = path;
        m_config = Config(m_path);
        emit pathChanged(path);
    }
}
