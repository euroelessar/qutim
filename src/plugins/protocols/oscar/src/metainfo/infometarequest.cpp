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

#include "infometarequest_p.h"
#include "../icqaccount.h"
#include "../icqcontact.h"
#include "metafields_p.h"
#include <QDate>

namespace qutim_sdk_0_3 {

namespace oscar {

void ShortInfoMetaRequestPrivate::readString(MetaFieldEnum value, const DataUnit &data)
{
	QString str = AbstractMetaRequest::readSString(data);
	if (!str.isEmpty())
		values.insert(value, str);
}

void ShortInfoMetaRequestPrivate::readFlag(MetaFieldEnum value, const DataUnit &data)
{
	bool f = static_cast<bool>(data.read<quint8>());
	values.insert(value, f);
}

void ShortInfoMetaRequestPrivate::dump()
{
	QHashIterator<MetaField, QVariant> itr(values);
	while (itr.hasNext()) {
		itr.next();
		if (itr.value().canConvert<CategoryList>())
			qDebug() << itr.key().toString() << itr.value().value<CategoryList>();
		else
			qDebug() << itr.key().toString() << itr.value();
	}
}

ShortInfoMetaRequest::ShortInfoMetaRequest(IcqAccount *account) :
	AbstractMetaRequest(account, new ShortInfoMetaRequestPrivate)
{
	Q_D(ShortInfoMetaRequest);
	d->uin = account->id().toUInt();
}

ShortInfoMetaRequest::ShortInfoMetaRequest(IcqContact *contact) :
		AbstractMetaRequest(contact->account(), new ShortInfoMetaRequestPrivate)
{
	Q_D(ShortInfoMetaRequest);
	d->uin = contact->id().toUInt();
}

MetaInfoValuesHash ShortInfoMetaRequest::values() const
{
	return d_func()->values;
}

QVariant ShortInfoMetaRequest::value(MetaField value, const QVariant &def) const
{
	return d_func()->values.value(value, def);
}

void ShortInfoMetaRequest::send() const
{
	Q_D(const ShortInfoMetaRequest);
	DataUnit data;
	data.append<quint32>(d->uin, LittleEndian);
	sendRequest(0x04ba, data);
}

ShortInfoMetaRequest::ShortInfoMetaRequest()
{
}

bool ShortInfoMetaRequest::handleData(quint16 type, const DataUnit &data)
{
	Q_D(ShortInfoMetaRequest);
	if (type != 0x0104)
		return false;
	d->readString(Nick, data);
	d->readString(FirstName, data);
	d->readString(LastName, data);
	d->readString(Email, data);
	d->readFlag(AuthFlag, data);
	data.skipData(2); // 0x00 unknown
	{
		quint8 genderId = data.read<quint8>();
		if (genderId)
			d->values.insert(Gender, genders().value(genderId));
	}
	qDebug() << d->uin << "short info:";
	d->dump();
	close(true);
	return true;
}

template <typename T>
void FullInfoMetaRequestPrivate::readField(MetaFieldEnum value, const DataUnit &data, const FieldNamesList &list)
{
	QString str = list.value(data.read<T>(LittleEndian));
	if (!str.isEmpty())
		values.insert(value, str);
}

void FullInfoMetaRequestPrivate::readCatagories(MetaFieldEnum value, const DataUnit &data, const FieldNamesList &list)
{
	CategoryList result;
	quint8 count = data.read<quint8>();
	Category category;
	for (int i = 0; i < count; ++i) {
		category.category = list.value(data.read<quint16>(LittleEndian));
		category.keyword = AbstractMetaRequest::readSString(data);
		if (!category.category.isEmpty())
			result << category;
	}
	if (!result.isEmpty())
		values.insert(value, QVariant::fromValue(result));
}

void FullInfoMetaRequestPrivate::handleBasicInfo(const DataUnit &data)
{
	readString(Nick, data);
	readString(FirstName, data);
	readString(LastName, data);
	readString(Email, data);
	readString(HomeCity, data);
	readString(HomeState, data);
	readString(HomePhone, data);
	readString(HomeFax, data);
	readString(HomeAddress, data);
	readString(CellPhone, data);
	readString(HomeZipCode, data);
	readField<quint16>(HomeCountry, data, countries());
	values.insert(GMT, data.read<qint8>());
	readFlag(AuthFlag, data);
	readFlag(WebawareFlag, data);
	readFlag(DirectConnectionFlag, data);
	readFlag(PublishPrimaryEmailFlag, data);
}

void FullInfoMetaRequestPrivate::handleMoreInfo(const DataUnit &data)
{
	quint16 age = data.read<quint16>(LittleEndian);
	if (age != 0)
		values.insert(Age, age);
	{
		quint8 genderId = data.read<quint8>();
		if (genderId)
			values.insert(Gender, genders().value(genderId));
	}
	readString(Homepage, data);
	{
		quint16 y = data.read<quint16>(LittleEndian);
		quint8 m = data.read<quint8>();
		quint8 d =  data.read<quint8>();
		if (QDate::isValid(y, m, d))
			values.insert(Birthday, QDate(y, m, d));
	}
	{
		QStringList langList;
		for (int i = 0; i < 3; ++i) {
			QString lang = languages().value(data.read<quint8>());
			if (!lang.isEmpty())
				langList << lang;
		}
		if (!langList.isEmpty())
			values.insert(Languages, langList);
	}
	data.skipData(2); // 0x0000 unknown
	readString(OriginalCity, data);
	readString(OriginalState, data);
	readField<quint16>(OriginalCountry, data, countries());
	values.insert(GMT, data.read<qint8>());
}

void FullInfoMetaRequestPrivate::handleEmails(const DataUnit &data)
{
	QStringList emails;
	quint8 count = data.read<quint8>();
	for (int i = 0; i < count; ++i) {
		bool isPublish = data.read<quint8>();
		Q_UNUSED(isPublish);
		QString email = AbstractMetaRequest::readSString(data);
		if (!email.isEmpty())
			emails << email;
	}
	if (!emails.isEmpty())
		values.insert(Emails, emails);
}

void FullInfoMetaRequestPrivate::handleHomepage(const DataUnit &data)
{
	bool isEnabled = data.read<quint8>();
	Q_UNUSED(isEnabled);
	quint16 homepageCategoryCode = data.read<quint16>(LittleEndian);
	Q_UNUSED(homepageCategoryCode);
	readString(Homepage, data);
}

void FullInfoMetaRequestPrivate::handleWork(const DataUnit &data)
{
	readString(WorkCity, data);
	readString(WorkState, data);
	readString(WorkPhone, data);
	readString(WorkFax, data);
	readString(WorkAddress, data);
	readString(WorkZip, data);
	readField<quint16>(WorkCountry, data, countries());
	readString(WorkCompany, data);
	readString(WorkDepartment, data);
	readString(WorkPosition, data);
	readField<quint16>(WorkOccupation, data, occupations());
	readString(WorkWebpage, data);
}


FullInfoMetaRequest::FullInfoMetaRequest(IcqAccount *account) :
	ShortInfoMetaRequest(account)
{
}

FullInfoMetaRequest::FullInfoMetaRequest(IcqContact *contact) :
	ShortInfoMetaRequest(contact)
{
}

void FullInfoMetaRequest::send() const
{
	Q_D(const FullInfoMetaRequest);
	DataUnit data;
	data.append<quint32>(d->uin, LittleEndian);
	sendRequest(0x04b2, data);
}

FullInfoMetaRequest::FullInfoMetaRequest()
{
}

bool FullInfoMetaRequest::handleData(quint16 type, const DataUnit &data)
{
	Q_D(FullInfoMetaRequest);
	switch (type) {
	case (StateBasicInfo):
		d->handleBasicInfo(data);
		break;
	case (StateMoreInfo):
		d->handleMoreInfo(data);
		break;
	case (StateEmails):
		d->handleEmails(data);
		break;
	case (StateHomepage):
		d->handleHomepage(data);
		break;
	case (StateWork):
		d->handleWork(data);
		break;
	case (StateNotes):
		d->readString(Notes, data);
		break;
	case (StateInterests):
		d->readCatagories(Interests, data, interests());
		break;
	case (StateAffilations): {
		d->readCatagories(Pasts, data, pasts());
		d->readCatagories(Affilations, data, affilations());
		break;
	}
	default:
		close(false, ProtocolError, tr("Incorrect format of the metarequest"));
		return false;
	}
	emit infoUpdated(static_cast<State>(type));
	if (type == StateAffilations) {
		close(true);
		qDebug() << d->uin << "full info:";
		d->dump();
	}
	return true;
}

} } // namespace qutim_sdk_0_3::oscar

