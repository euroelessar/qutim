#include "quickconfig.h"
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QDebug>

using namespace qutim_sdk_0_3;

QuickConfig::QuickConfig(QObject *parent) :
    QObject(parent)
{
}

QVariant QuickConfig::value(const QString &key, const QVariant &def, ValueFlags type)
{
	qDebug() << "value" << key << def << type;
	QVariant result = m_config.value(key, def, static_cast<Config::ValueFlag>(int(type)));
	qDebug() << Q_FUNC_INFO << result;
	return result;
}

void QuickConfig::setValue(const QString &key, const QVariant &value, ValueFlags type)
{
	qDebug() << "setValue" << key << value << type;
    m_config.setValue(key, value, static_cast<Config::ValueFlag>(int(type)));
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

QObject *QuickConfig::object() const
{
	return m_object;
}

void QuickConfig::setObject(QObject *object)
{
	if (m_object != object) {
		if (!m_path.isEmpty()) {
			m_path = QString();
			emit pathChanged(m_path);
		}
		if (Account *account = qobject_cast<Account*>(object))
			m_config = account->config();
		else if (Protocol *protocol = qobject_cast<Protocol*>(object))
			m_config = protocol->config();
		else
			m_config = Config(QVariantMap());
		m_object = object;
		emit objectChanged(object);
	}
}
