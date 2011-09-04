/****************************************************************************
 *  metafield.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef METAFIELD_H
#define METAFIELD_H

#include <QHash>
#include "qutim/inforequest.h"
#include "../icq_global.h"

namespace qutim_sdk_0_3 {

namespace oscar {

struct Category
{
	QString category;
	QString keyword;
};
typedef QList<Category> CategoryList;
typedef QHash<quint16, QString> FieldNamesList;
typedef QHash<quint32, QString> AgesList;

enum GenderEnum
{
	Unknown,
	Female = 1,
	Male = 2,
	Female2 = 'F',
	Male2 = 'M'
};

enum MetaFieldEnum {
	Uin,
	// Basic info
	Nick,
	FirstMetaField = Nick,
	FirstName,
	LastName,
	Email,
	HomeCity,
	HomeState,
	HomePhone,
	HomeFax,
	HomeAddress,
	CellPhone,
	HomeZipCode,
	HomeCountry,
	GMT,
	AuthFlag,
	WebawareFlag,
	DirectConnectionFlag,
	PublishPrimaryEmailFlag,
	// More info
	Age,
	Gender,
	Homepage,
	Birthday,
	Languages,
	OriginalCity,
	OriginalState,
	OriginalCountry,
	// Work info
	WorkCity,
	WorkState,
	WorkPhone,
	WorkFax,
	WorkAddress,
	WorkZip,
	WorkCountry,
	WorkCompany,
	WorkDepartment,
	WorkPosition,
	WorkOccupation,
	WorkWebpage,
	// Other
	Emails,
	Notes,
	Interests,
	Pasts,
	Affilations,
	LastMetaField = Affilations,

	AgeRange,
	Whitepages,
	OnlineFlag
};

class LIBOSCAR_EXPORT MetaField
{
public:
	MetaField(const QString &name);
	MetaField(int value);
	MetaField(const MetaField &field);
	QString name() const;
	DataItem toDataItem(const QVariant &data = QVariant(), bool allowMultiItems = true) const;
	LocalizedString group() const;
	QList<LocalizedString> titleAlternatives() const;
	QList<LocalizedString> alternatives() const;
	QVariant defaultValue() const;
	MetaFieldEnum value() const { return m_value; };
	QString toString() const;
	operator QString() const { return toString(); }
	MetaField &operator=(int value) { m_value = static_cast<MetaFieldEnum>(value); return *this; }
	bool operator==(int value) const { return m_value == value; }
	bool operator!=(int value) const { return m_value != value; }
	bool operator==(const MetaField &value) const { return m_value == value.m_value; }
	bool operator!=(const MetaField &value) const { return m_value != value.m_value; }
	static QHash<MetaField, QVariant> dataItemToHash(const DataItem &items, bool allItems = false);
private:
	mutable QString m_name;
	MetaFieldEnum m_value;
};
typedef QHash<MetaField, QVariant> MetaInfoValuesHash;

QDebug operator<<(QDebug dbg, const Category &cat);

} } // namespace qutim_sdk_0_3::oscar

inline uint qHash(const qutim_sdk_0_3::oscar::MetaField &field)
{
	return qHash(static_cast<int>(field.value()));
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::Category);
Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::CategoryList);


#endif // METAFIELD_H
