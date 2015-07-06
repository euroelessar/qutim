#include "quickconfig.h"
#include <qutim/account.h>
#include <qutim/protocol.h>

namespace qutim_sdk_0_3 {

QuickConfig::QuickConfig(QObject *parent) :
	QObject(parent)
{
	setObjectName(QStringLiteral("QuickConfig"));
}

void QuickConfig::setPath(const QString &path)
{
	if (m_path != path) {
		m_path = path;
		m_config = Config(path);

		if (!m_group.isEmpty())
			m_config.beginGroup(m_group);

		emit pathChanged();
	}
}

QString QuickConfig::path() const
{
	return m_path;
}

void QuickConfig::setGroup(const QString &group)
{
	if (m_group != group) {
		if (!m_group.isEmpty())
			m_config.endGroup();

		m_group = group;
		if (!group.isEmpty())
			m_config.beginGroup(group);
	}
}

QString QuickConfig::group() const
{
	return m_group;
}

QObject *QuickConfig::object() const
{
	return m_object;
}

void QuickConfig::setObject(QObject *object)
{
	if (m_object != object) {
		m_object = object;

		if (Account *account = qobject_cast<Account *>(m_object))
			m_config = account->config();
		else if (Protocol *protocol = qobject_cast<Protocol *>(m_object))
			m_config = protocol->config();
		else
			m_config = Config(m_path);

		if (!m_group.isEmpty())
			m_config.beginGroup(m_group);

		emit objectChanged(object);
	}
}

QVariant QuickConfig::value(const QString &name, const QVariant &defaultValue)
{
	return m_config.value(name, defaultValue);
}

void QuickConfig::setValue(const QString &name, const QVariant &value)
{
	m_config.setValue(name, value);
}

void QuickConfig::beginGroup(const QString &name)
{
	m_config.beginGroup(name);
}

void QuickConfig::endGroup()
{
	m_config.endGroup();
}

void QuickConfig::forceSync()
{
	m_config.sync();
}

void QuickConfig::classBegin()
{
}

void QuickConfig::componentComplete()
{
	syncProperties(this);
}

void QuickConfig::syncProperties(QObject *object)
{
	const QMetaObject * const base = object == this ? &staticMetaObject : &QObject::staticMetaObject;
	const QMetaObject * const actual = object->metaObject();

	int propertiesCount = actual->propertyCount();
	for (int index = base->propertyCount(); index < propertiesCount; ++index) {
		QMetaProperty property = actual->property(index);
		QVariant defaultValue = property.read(object);
		QMetaType type(property.userType());

		if (type.flags() & QMetaType::PointerToQObject) {
			QObject *subObject = defaultValue.value<QObject *>();
			if (!subObject) {
				qWarning() << "QuickConfig: null value at" << this << ", property:" << property.name();
				continue;
			}
			m_config.beginGroup(QLatin1String(property.name()));
			syncProperties(subObject);
			m_config.endGroup();
			continue;
		}

		ConfigValue<QVariant> actualValue = m_config.value(QLatin1String(property.name()), defaultValue);
		property.write(object, actualValue.value());

		// Store actualValue as it's destruction will drop onChange listener
		new QuickConfigListener(m_path, m_group, property, object, actualValue, this);

		actualValue.onChange(object, [object, property, defaultValue] (const QVariant &value) {
			property.write(object, value.isValid() ? value : defaultValue);
		});
	}
}

QuickConfigListener::QuickConfigListener(const QString &path, const QString &group,
										 const QMetaProperty &property, QObject *object,
										 const ConfigValue<QVariant> &value, QuickConfig *parent)
	: QObject(parent), m_path(path), m_group(group), m_property(property), m_object(object), m_value(value)
{
	static int slotIndex = staticMetaObject.indexOfMethod("onPropertyChanged()");
	QMetaMethod slot = staticMetaObject.method(slotIndex);
	connect(m_object, property.notifySignal(), this, slot);
}

void QuickConfigListener::onPropertyChanged()
{
	QVariant value = m_property.read(m_object);
	Config config(m_path);
	config.beginGroup(m_group);
	config.setValue(QLatin1String(m_property.name()), value);
}

} // namespace qutim_sdk_0_3
