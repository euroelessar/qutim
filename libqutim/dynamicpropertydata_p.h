#ifndef DYNAMICPROPERTYDATA_P_H
#define DYNAMICPROPERTYDATA_P_H

#include <QSharedData>
#include <QVariant>
#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class DynamicPropertyData;

	namespace CompiledProperty
	{
		typedef QVariant (DynamicPropertyData::*Getter)() const;
		typedef void (DynamicPropertyData::*Setter)(const QVariant &variant);
	}

	class DynamicPropertyData : public QSharedData
	{
	public:
		typedef CompiledProperty::Getter Getter;
		typedef CompiledProperty::Setter Setter;
		DynamicPropertyData() {}
		DynamicPropertyData(const DynamicPropertyData &o) :
				QSharedData(o), names(o.names), values(o.values) {}
		QList<QByteArray> names;
		QList<QVariant> values;

		QVariant property(const char *name, const QVariant &def, const QList<QByteArray> &names,
						  const QList<Getter> &getters) const;
		void setProperty(const char *name, const QVariant &value, const QList<QByteArray> &names,
						 const QList<Setter> &setters);
	};
}

#endif // DYNAMICPROPERTYDATA_P_H
