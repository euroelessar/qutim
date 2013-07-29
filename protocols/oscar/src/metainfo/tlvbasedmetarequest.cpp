/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "tlvbasedmetarequest_p.h"
#include "metafields_p.h"
#include <QDate>

namespace qutim_sdk_0_3 {

namespace oscar {

void TlvBasedMetaRequestPrivate::addString(quint16 id, MetaFieldEnum value, DataUnit &data, bool test) const
{
	if (!test || values.contains(value)) {
		DataUnit tlv;
		addString(values.value(value).toString(), tlv);
		data.appendTLV(id, tlv, LittleEndian);
	}
}

void TlvBasedMetaRequestPrivate::addString(const QString &str, DataUnit &data) const
{
	QByteArray d = Util::asciiCodec()->fromUnicode(str);
	data.append<quint16>(d.size() + 1, LittleEndian);
	data.append(d);
	data.append<quint8>(0);
}

template <typename T>
void TlvBasedMetaRequestPrivate::addField(quint16 id, MetaFieldEnum value, DataUnit &data, bool test) const
{
	if (!test || values.contains(value)) {
		DataUnit tlv;
        tlv.append(values.value(value).value<T>(), LittleEndian);
		data.appendTLV(id, tlv, LittleEndian);
	}
}

template <typename T>
void TlvBasedMetaRequestPrivate::addCategoryId(quint16 id, MetaFieldEnum value, DataUnit &data, const FieldNamesList &list) const
{
	if (values.contains(value)) {
		QStringList d = values.value(value).toStringList();
		if (!d.isEmpty()) {
			foreach (const QString &str, d) {
				DataUnit tlv;
				T catId = list.key(str);
				tlv.append<T>(catId, LittleEndian);
				data.appendTLV(id, tlv, LittleEndian);
			}
		} else {
			DataUnit tlv;
			tlv.append<T>(0, LittleEndian);
			data.appendTLV(id, tlv, LittleEndian);
		}
	}
}

void TlvBasedMetaRequestPrivate::addCategory(quint16 id, MetaFieldEnum value, DataUnit &data, const FieldNamesList &list) const
{
	if (values.contains(value)) {
		QVariant val = values.value(value);
		CategoryList categories;
		if (val.canConvert<Category>())
			categories << val.value<Category>();
		else
			categories = val.value<CategoryList>();
		if (!categories.isEmpty()) {
			foreach (const Category &cat, categories) {
				DataUnit tlv;
				quint16 catId = list.key(cat.category);
				tlv.append<quint16>(catId, LittleEndian);
				addString(cat.keyword, tlv);
				data.appendTLV(id, tlv, LittleEndian);
			}
		} else {
			DataUnit tlv;
			tlv.append<quint16>(0, LittleEndian);
			addString("", tlv);
			data.appendTLV(id, tlv, LittleEndian);
		}
	}
}

TlvBasedMetaRequest::TlvBasedMetaRequest(IcqAccount *account, TlvBasedMetaRequestPrivate *d) :
	AbstractMetaRequest(account, d)
{
}

void TlvBasedMetaRequest::setValue(const MetaField &field, const QVariant &value)
{
	d_func()->values.insert(field, value);
}

void TlvBasedMetaRequest::sendTlvBasedRequest(quint16 type) const
{
	Q_D(const TlvBasedMetaRequest);
	DataUnit data;
	if (d->values.contains(Uin)){
		DataUnit tlv;
		quint32 uin = d->values.value(Uin).toUInt();
		tlv.append<quint32>(uin, LittleEndian);
		data.appendTLV(0x0136, tlv, LittleEndian);
	}
	d->addString(0x0140, FirstName, data);
	d->addString(0x014A, LastName, data);
	d->addString(0x0154, Nick, data);
	d->addString(0x015E, Email, data);
	if (d->values.contains(AgeRange)) {
		DataUnit tlv;
		QString rangeStr = d->values.value(AgeRange).toString();
		quint32 range = ages().key(rangeStr, 0);
		if (range) {
			tlv.append<quint32>(range, LittleEndian);
			data.appendTLV(0x0168, tlv, LittleEndian);
		}
	}
	d->addField<quint16>(0x0172, Age, data);
	if (d->values.contains(Gender)) {
		DataUnit tlv;
		GenderEnum gender = static_cast<GenderEnum>(genders().key(d->values.value(Gender).toString()));
		tlv.append<quint8>(gender, LittleEndian);
		data.appendTLV(0x017C, tlv, LittleEndian);
	}
	d->addCategoryId<quint16>(0x0186, Languages, data, languages());
	d->addString(0x0190, HomeCity, data);
	d->addString(0x019A, HomeState, data);
	d->addCategoryId<quint16>(0x01A4, HomeCountry, data, countries());
	d->addString(0x01AE, WorkCompany, data);
	d->addString(0x01B8, WorkDepartment, data);
	d->addString(0x01C2, WorkPosition, data);
	d->addCategoryId<quint16>(0x01CC, WorkOccupation, data, occupations());
	d->addCategory(0x01D6, Affilations, data, affilations());
	d->addCategory(0x01EA, Interests, data, interests());
	d->addCategory(0x01FE, Pasts, data, pasts());
	//d->addCategory(0x0212, Homepage, tlvs, ...);
	if (d->values.contains(Homepage)){
		DataUnit tlv;
		tlv.append<quint16>(0, LittleEndian); // category ?
		d->addString(d->values.value(Homepage).toString(), tlv);
		data.appendTLV(0x0212, tlv, LittleEndian);
	}
	d->addString(0x0226, Whitepages, data);
	d->addField<quint8>(0x0230, OnlineFlag, data);
	if (d->values.contains(Birthday)) {
		DataUnit tlv;
		QDate date = d->values.value(Birthday).toDate();
		tlv.append<quint16>(date.year(), LittleEndian);
		tlv.append<quint16>(date.month(), LittleEndian);
		tlv.append<quint16>(date.day(), LittleEndian);
		data.appendTLV(0x023A, tlv, LittleEndian);
	}
	d->addString(0x0258, Notes, data);
	d->addString(0x0262, HomeAddress, data);
	d->addField<quint32>(0x026C, HomeZipCode, data);
	d->addString(0x0276, HomePhone, data);
	d->addString(0x0280, HomeFax, data);
	d->addString(0x028A, CellPhone, data);
	d->addString(0x0294, WorkAddress, data);
	d->addString(0x029E, WorkCity, data);
	d->addString(0x02A8, WorkState, data);
	d->addCategoryId<quint16>(0x02B2, WorkCountry, data, countries());
	d->addField<quint32>(0x02BC, WorkZip, data);
	d->addString(0x02C6, WorkPhone, data);
	d->addString(0x02D0, WorkFax, data);
	d->addString(0x02DA, WorkWebpage, data);
	d->addField<quint8>(0x02F8, WebawareFlag, data);
	d->addField<quint8>(0x030C, AuthFlag, data);
	d->addField<quint8>(0x0316, GMT, data);
	d->addString(0x0320, OriginalCity, data);
	d->addString(0x032A, OriginalState, data);
	d->addCategoryId<quint16>(0x0334, OriginalCountry, data, countries());
	sendRequest(type, data);
}

} } // namespace qutim_sdk_0_3::oscar

