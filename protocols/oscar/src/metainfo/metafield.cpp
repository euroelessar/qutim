/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "metafield.h"
#include "metafields_p.h"

namespace qutim_sdk_0_3 {

namespace oscar {

static LocalizedString notSpecifiedStr = QT_TRANSLATE_NOOP("MetaInfo", "Not specified");

QDebug operator<<(QDebug dbg, const Category &cat)
{
	dbg.nospace() << "{" << cat.category << ", " << cat.keyword << "}";
	return dbg.space();
}

MetaField::MetaField(const QString &name)
{
	m_name = name;
	m_value = static_cast<MetaFieldEnum>(fields_names().key(name));
}

MetaField::MetaField(int value) :
		m_value(static_cast<MetaFieldEnum>(value))
{
}

MetaField::MetaField(const MetaField &field) :
		m_value(field.m_value)
{
}

QString MetaField::name() const
{
	if (m_name.isEmpty())
		m_name = fields_names().value(m_value);
	return m_name;
}

DataItem MetaField::toDataItem(const QVariant &data_helper, bool allowMultiItems) const
{
	DataItem item;
	item.setName(name());
	LocalizedStringList alt = titleAlternatives();
	if (!alt.isEmpty())
		item.setProperty("titleAlternatives", QVariant::fromValue(alt));
	alt = alternatives();
	if (!alt.isEmpty())
		item.setProperty("alternatives", QVariant::fromValue(alt));
	DataItem def = item;
	QVariant data = data_helper;
	if (data.isNull()) {
		data = defaultValue();
		item.setProperty("notSet", true);
	}
	item.setTitle(toString());
	if (allowMultiItems) {
		if (data.type() == QVariant::StringList)
			item.setProperty("maxStringsCount", 3);
		if (data.canConvert<CategoryList>()) {
			foreach (const Category &cat, data.value<CategoryList>()) {
				DataItem subitem = def;
				subitem.setName(name());
				subitem.setTitle(cat.category);
				subitem.setData(cat.keyword);
				item.addSubitem(subitem);
			}
			def.setTitle(QString());
			item.allowModifySubitems(def, 3);
		} else {
			item.setData(data);
		}
	} else {
		if (data.type() == QVariant::StringList || data.canConvert<CategoryList>())
			data = QVariant::String;
		item.setData(data);
	}
	return item;
}

LocalizedString MetaField::group() const
{
	if (m_value >= Nick && m_value <= PublishPrimaryEmailFlag)
		return QT_TRANSLATE_NOOP("MetaInfo", "Basic");
	else if (m_value >= Age && m_value <= OriginalCountry)
		return QT_TRANSLATE_NOOP("MetaInfo", "More");
	else if (m_value >= WorkCity && m_value <= WorkWebpage)
		return QT_TRANSLATE_NOOP("MetaInfo", "Work");
	else if (m_value == Pasts || m_value == Affilations)
		return QT_TRANSLATE_NOOP("MetaInfo", "Affilations");
	else
		return fields().value(m_value);
}

template <typename T>
		static inline QList<LocalizedString> getAlternativesList(const T &list)
{
	QList<LocalizedString> r;
	foreach (const QString &str, list)
		r << str;
	qSort(r);
	r.push_front(notSpecifiedStr);
	return r;
}

QList<LocalizedString> MetaField::titleAlternatives() const
{
	if (m_value == Affilations)
		return getAlternativesList(affilations());
	else if (m_value == Interests)
		return getAlternativesList(interests());
	else if (m_value == Pasts)
		return getAlternativesList(pasts());
	return QList<LocalizedString>();
}

QList<LocalizedString> MetaField::alternatives() const
{
	if (m_value == HomeCountry || m_value == OriginalCountry || m_value == WorkCountry) {
		return getAlternativesList(countries());
	} else if (m_value == WorkOccupation) {
		return getAlternativesList(occupations());
	} else if (m_value == Languages) {
		return getAlternativesList(languages());
	}else if (m_value == Gender) {
		QList<LocalizedString>  list;
		list << genders().value(Male);
		list << genders().value(Female);
		return list;
	} else if (m_value == AgeRange) {
		return getAlternativesList(ages());
	}
	return QList<LocalizedString>();
}

QVariant MetaField::defaultValue() const
{
	if (m_value == Languages)
		return QStringList();
	else if (m_value == Interests)
		return QVariant::fromValue(CategoryList());
	else if (m_value == Age)
		return QVariant::Int;
	else if ((m_value >= AuthFlag && m_value <= PublishPrimaryEmailFlag) || m_value == OnlineFlag)
		return QVariant::Bool;
	else if (m_value == Gender)
		return genders().value(0);
	else
		return QString();
}

QString MetaField::toString() const
{
	return fields().value(m_value);
}

static void dataItemToHashHelper(const DataItem &items, MetaInfoValuesHash &hash, bool allItems)
{
	foreach (const DataItem &item, items.subitems()) {
		if (item.isAllowedModifySubitems()) {
			if (allItems || item.hasSubitems()) {
				CategoryList list;
				foreach (const DataItem &catItem, item.subitems()) {
					Category cat;
					cat.category = catItem.title();
					cat.keyword = catItem.data().toString();
					list << cat;
				}
				hash.insert(item.name(), QVariant::fromValue(list));
			}
		} else if (item.hasSubitems()) {
			dataItemToHashHelper(item, hash, allItems);
		} else {
			static QSet<MetaFieldEnum> hasTitleAlternatives = QSet<MetaFieldEnum>()
															  << Affilations << Interests << Pasts;
			static QSet<MetaFieldEnum> alternatives = QSet<MetaFieldEnum>()
													  << HomeCountry << OriginalCountry << WorkCountry
													  << WorkOccupation << Languages << Gender
													  << AgeRange;

			MetaField field(item.name());
			if (hasTitleAlternatives.contains(field.value())) {
				Category cat;
				cat.category = item.title();
				if (cat.category == notSpecifiedStr)
					cat.category.clear();
				cat.keyword = item.data().toString();
				if (allItems || !cat.category.isEmpty())
					hash.insert(field, QVariant::fromValue(cat));
			} else {
				QVariant data = item.data();
				if (alternatives.contains(field.value()) && data.toString() == notSpecifiedStr)
					data.clear();
				if (allItems || !data.isNull())
					hash.insert(field, item.data());
			}
		}
	}
}

MetaInfoValuesHash MetaField::dataItemToHash(const DataItem &items, bool allItems)
{
	Q_ASSERT(!items.isNull());
	MetaInfoValuesHash hash;
	dataItemToHashHelper(items, hash, allItems);
	return hash;
}


} } // namespace qutim_sdk_0_3::oscar

