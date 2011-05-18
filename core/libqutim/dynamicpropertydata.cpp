#include "dynamicpropertydata_p.h"

namespace qutim_sdk_0_3
{
	QVariant DynamicPropertyData::property(const char *name, const QVariant &def,
										   const QList<QByteArray> &gNames,
										   const QList<Getter> &gGetters) const
	{
		QByteArray prop = QByteArray::fromRawData(name, strlen(name));
		int id = gNames.indexOf(prop);
		if (id < 0) {
			id = names.indexOf(prop);
			if(id < 0)
				return def;
			return values.at(id);
		}
		return (this->*gGetters.at(id))();
	}

	void DynamicPropertyData::setProperty(const char *name, const QVariant &value,
										  const QList<QByteArray> &gNames,
										  const QList<Setter> &gSetters)
	{
		QByteArray prop = QByteArray::fromRawData(name, strlen(name));
		int id = gNames.indexOf(prop);
		if (id < 0) {
			id = names.indexOf(prop);
			if (!value.isValid()) {
				if(id < 0)
					return;
				names.removeAt(id);
				values.removeAt(id);
			} else {
				if (id < 0) {
					prop.detach();
					names.append(prop);
					values.append(value);
				} else {
					values[id] = value;
				}
			}
		} else {
			(this->*gSetters.at(id))(value);
		}
	}
}
