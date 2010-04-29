/****************************************************************************
 *  metainfo.cpp
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

#include "metainfo_p.h"
#include "icqaccount.h"
#include "icqcontact.h"
#include <icqprotocol.h>
#include "oscarconnection.h"
#include <metainfofields_p.h>
#include <QStringList>
#include <QDate>
#include <QEventLoop>

namespace qutim_sdk_0_3 {

namespace oscar {

static QString readSString(const DataUnit &data)
{
	QString str = data.read<QString, quint16>(Util::asciiCodec(), LittleEndian);
	if (str.endsWith(QChar('\0')))
		str.chop(1);
	return str;
}

QDebug operator<<(QDebug dbg, const Category &cat)
{
	dbg.nospace() << "{" << cat.category << ", " << cat.keyword << "}";
	return dbg.space();
}

MetaInfo *MetaInfo::self = 0;

MetaInfoField::MetaInfoField(const QString &name)
{
	m_name = name;
	m_value = static_cast<MetaInfoFieldEnum>(fields_names()->key(name));
}

MetaInfoField::MetaInfoField(int value) :
	m_value(static_cast<MetaInfoFieldEnum>(value))
{
}

MetaInfoField::MetaInfoField(const MetaInfoField &field) :
	m_value(field.m_value)
{
}

QString MetaInfoField::name() const
{
	if (m_name.isEmpty())
		m_name = fields_names()->value(m_value);
	return m_name;
}

LocalizedString MetaInfoField::group() const
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
		return fields()->value(m_value);
}

static inline QList<LocalizedString> getAlternativesList(const FieldNamesList &list)
{
	QList<LocalizedString> r;
	foreach (const QString &str, list)
		r << str;
	return r;
}

QList<LocalizedString> MetaInfoField::titleAlternatives() const
{
	if (m_value == Affilations)
		 return getAlternativesList(*affilations());
	else if (m_value == Interests)
		return getAlternativesList(*interests());
	else if (m_value == Pasts)
		return getAlternativesList(*pasts());
	return QList<LocalizedString>();
}

QList<LocalizedString> MetaInfoField::alternatives() const
{
	if (m_value == HomeCountry || m_value == OriginalCountry || m_value == WorkCountry)
		return getAlternativesList(*countries());
	else if (m_value == WorkOccupation)
		return getAlternativesList(*occupations());
	else if (m_value == Languages)
		return getAlternativesList(*languages());
	else if (m_value == Gender)
		return getAlternativesList(*genders());
	return QList<LocalizedString>();
}

QVariant MetaInfoField::defaultValue() const
{
	if (m_value == Languages)
		return QStringList();
	else if (m_value == Interests)
		return QVariant::fromValue(CategoryList());
	else if (m_value == Age)
		return QVariant::Int;
	else if (m_value >= AuthFlag && m_value <= PublishPrimaryEmailFlag)
		return QVariant::Bool;
	else if (m_value == Gender)
		return genders()->value(0);
	else
		return QString();
}

QString MetaInfoField::toString() const
{
	return fields()->value(m_value);
}

AbstractMetaInfoRequest::AbstractMetaInfoRequest()
{
}

AbstractMetaInfoRequest::~AbstractMetaInfoRequest()
{
	cancel();
}

quint16 AbstractMetaInfoRequest::id() const
{
	return d_func()->id;
}

IcqAccount *AbstractMetaInfoRequest::account() const
{
	return d_func()->account;
}

bool AbstractMetaInfoRequest::isDone() const
{
	return d_func()->ok;
}

void AbstractMetaInfoRequest::setTimeout(int msec)
{
	d_func()->timer.setInterval(msec);
}

void AbstractMetaInfoRequest::cancel()
{
	close(false);
}

AbstractMetaInfoRequest::AbstractMetaInfoRequest(IcqAccount *account, AbstractMetaInfoRequestPrivate *d) :
	d_ptr(d)
{
	d->id = MetaInfo::instance().nextId();
	d->account = account;
	d->ok = false;
	d->timer.setInterval(60000);
	d->timer.setSingleShot(true);
	connect(&d->timer, SIGNAL(timeout()), this, SLOT(cancel()));
}

void AbstractMetaInfoRequest::sendRequest(quint16 type, const DataUnit &extendData) const
{
	Q_D(const AbstractMetaInfoRequest);
	SNAC snac(ExtensionsFamily, ExtensionsMetaCliRequest);
	DataUnit tlvData;
	DataUnit data;
	data.append<quint32>(d->account->id().toUInt(), LittleEndian);
	data.append<quint16>(0x07d0, LittleEndian);
	data.append<quint16>(d->id, LittleEndian);
	data.append<quint16>(type, LittleEndian);
	data.append(extendData.data());
	tlvData.append<quint16>(data.data().size(), LittleEndian);
	tlvData.append(data.data());
	snac.appendTLV(1, tlvData);
	MetaInfo::instance().addRequest(const_cast<AbstractMetaInfoRequest*>(this));
	d->account->connection()->send(snac);
	d->timer.start();
}

void AbstractMetaInfoRequest::close(bool ok)
{
	Q_D(AbstractMetaInfoRequest);
	d->ok = ok;
	if (MetaInfo::instance().removeRequest(this) || ok)
		emit done(ok);
}

void ShortInfoMetaRequestPrivate::readString(MetaInfoFieldEnum value, const DataUnit &data)
{
	QString str = readSString(data);
	if (!str.isEmpty())
		values.insert(value, str);
}

void ShortInfoMetaRequestPrivate::readFlag(MetaInfoFieldEnum value, const DataUnit &data)
{
	bool f = static_cast<bool>(data.read<quint8>());
	values.insert(value, f);
}

void ShortInfoMetaRequestPrivate::dump()
{
	QHashIterator<MetaInfoField, QVariant> itr(values);
	while (itr.hasNext()) {
		itr.next();
		if (itr.value().canConvert<CategoryList>())
			debug() << itr.key().toString() << itr.value().value<CategoryList>();
		else
			debug() << itr.key().toString() << itr.value();
	}
}

ShortInfoMetaRequest::ShortInfoMetaRequest(IcqAccount *account, IcqContact *contact) :
	AbstractMetaInfoRequest(account, new ShortInfoMetaRequestPrivate)
{
	Q_D(ShortInfoMetaRequest);
	if (contact)
		d->uin = contact->id().toUInt();
	else
		d->uin = account->id().toUInt();
}

MetaInfoValuesHash ShortInfoMetaRequest::values() const
{
	return d_func()->values;
}

QVariant ShortInfoMetaRequest::value(MetaInfoField value, const QVariant &def) const
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
			d->values.insert(Gender, genders()->value(genderId));
	}
	debug() << d->uin << "short info:";
	d->dump();
	close(true);
	return true;
}

void FullInfoMetaRequestPrivate::readCatagories(MetaInfoFieldEnum value, const DataUnit &data, FieldNamesList *list)
{
	CategoryList result;
	quint8 count = data.read<quint8>();
	Category category;
	for (int i = 0; i < count; ++i) {
		category.category = list->value(data.read<quint16>(LittleEndian));
		category.keyword = readSString(data);
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
			values.insert(Gender, genders()->value(genderId));
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
			QString lang = languages()->value(data.read<quint8>());
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
		QString email = readSString(data);
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


FullInfoMetaRequest::FullInfoMetaRequest(IcqAccount *account, IcqContact *contact) :
	ShortInfoMetaRequest(account, contact)
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
		close(false);
		return false;
	}
	emit infoUpdated(static_cast<State>(type));
	if (type == StateAffilations) {
		close(true);
		debug() << d->uin << "full info:";
		d->dump();
	}
	return true;
}

void TlvBasedMetaInfoRequestPrivate::addString(quint16 id, MetaInfoFieldEnum value, DataUnit &data, bool test) const
{
	if (!test || values.contains(value)) {
		DataUnit tlv;
		addString(values.value(value).toString(), tlv);
		data.appendTLV(id, tlv, LittleEndian);
	}
}

void TlvBasedMetaInfoRequestPrivate::addString(const QString &str, DataUnit &data) const
{
	QByteArray d = Util::asciiCodec()->fromUnicode(str);
	data.append<quint16>(d.size() + 1, LittleEndian);
	data.append(d);
	data.append<quint8>(0);
}

template <typename T>
void TlvBasedMetaInfoRequestPrivate::addField(quint16 id, MetaInfoFieldEnum value, DataUnit &data, bool test) const
{
	if (!test || values.contains(value)) {
		DataUnit tlv;
		tlv.append(qVariantValue<T>(values.value(value)), LittleEndian);
		data.appendTLV(id, tlv, LittleEndian);
	}
}

template <typename T>
void TlvBasedMetaInfoRequestPrivate::addCategoryId(quint16 id, MetaInfoFieldEnum value, DataUnit &data, FieldNamesList *list) const
{
	if (values.contains(value)) {
		foreach (const QString &str, values.value(value).toStringList()) {
			DataUnit tlv;
			T catId = list->key(str);
			tlv.append<T>(catId, LittleEndian);
			data.appendTLV(id, tlv, LittleEndian);
		}
	}
}

void TlvBasedMetaInfoRequestPrivate::addCategory(quint16 id, MetaInfoFieldEnum value, DataUnit &data, FieldNamesList *list) const
{
	if (values.contains(value)) {
		QVariant val = values.value(value);
		CategoryList categories;
		if (val.canConvert<Category>())
			categories << val.value<Category>();
		else
			categories = val.value<CategoryList>();
		foreach (const Category &cat, categories) {
			DataUnit tlv;
			quint16 catId = list->key(cat.category);
			tlv.append<quint16>(catId, LittleEndian);
			addString(cat.keyword, tlv);
			data.appendTLV(id, tlv, LittleEndian);
		}
	}
}

TlvBasedMetaInfoRequest::TlvBasedMetaInfoRequest(IcqAccount *account, TlvBasedMetaInfoRequestPrivate *d) :
	AbstractMetaInfoRequest(account, d)
{
}

void TlvBasedMetaInfoRequest::setValue(const MetaInfoField &field, const QVariant &value)
{
	d_func()->values.insert(field, value);
}

void TlvBasedMetaInfoRequest::sendTlvBasedRequest(quint16 type) const
{
	Q_D(const TlvBasedMetaInfoRequest);
	DataUnit data;
	{
		DataUnit tlv;
		quint32 uin = d->values.value(Uin).toUInt();
		tlv.append<quint32>(uin, LittleEndian);
		data.appendTLV(0x0136, tlv, LittleEndian);
	}
	d->addString(0x0140, FirstName, data);
	d->addString(0x014A, LastName, data);
	d->addString(0x0154, Nick, data);
	d->addString(0x015E, Email, data);
	if (d->values.contains(Ages)) {
		DataUnit tlv;
		AgeRange range = d->values.value(Age).value<AgeRange>();
		tlv.append<quint16>(range.first, LittleEndian);
		tlv.append<quint16>(range.second, LittleEndian);
		data.appendTLV(0x0168, tlv, LittleEndian);
	}
	d->addField<quint16>(0x0172, Age, data);
	if (d->values.contains(Gender)) {
		DataUnit tlv;
		GenderEnum gender = static_cast<GenderEnum>(genders()->key(d->values.value(Gender).toString()));
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
	{
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

UpdateAccountInfoMetaRequest::UpdateAccountInfoMetaRequest(IcqAccount *account, const MetaInfoValuesHash &values) :
	TlvBasedMetaInfoRequest(account, new UpdateAccountInfoMetaRequestPrivate)
{
	Q_D(UpdateAccountInfoMetaRequest);
	d->values = values;
}

void UpdateAccountInfoMetaRequest::send() const
{
	sendTlvBasedRequest(0x0C3A);
}

bool UpdateAccountInfoMetaRequest::handleData(quint16 type, const DataUnit &data)
{
	Q_UNUSED(data);
	if (type == 0x0c3f) {
		debug() << "Account info successfully has been updated";
		emit infoUpdated();
		return true;
	}
	return false;
}

FindContactsMetaRequest::FindContactsMetaRequest(IcqAccount *account) :
	TlvBasedMetaInfoRequest(account, new FindContactsMetaRequestPrivate)
{
}

void FindContactsMetaRequest::send() const
{
	Q_D(const FindContactsMetaRequest);
	quint16 type = 0x055F; // CLI_WHITE_PAGES_SEARCH2
	if (d->values.contains(Email))
		type = 0x0573; // META_SEARCH_BY_EMAIL
	if (d->values.contains(Uin))
		type = 0x0569; // META_SEARCH_BY_UIN
	sendTlvBasedRequest(type);
}

const QHash<QString, FindContactsMetaRequest::FoundContact> &FindContactsMetaRequest::contacts() const
{
	return d_func()->contacts;
}

bool FindContactsMetaRequest::handleData(quint16 type, const DataUnit &data)
{
	Q_D(FindContactsMetaRequest);
	if (type != 0x01A4 && type != 0x01AE)
		return false;
	data.skipData(2); // data size
	FoundContact contact;
	contact.uin = QString::number(data.read<quint32>(LittleEndian));
	contact.nick = readSString(data);
	contact.firstName = readSString(data);
	contact.lastName = readSString(data);
	contact.email = readSString(data);
	contact.authFlag = data.read<quint8>();
	contact.status = static_cast<FoundContact::Status>(data.read<quint16>(LittleEndian));
	contact.gender = genders()->value(data.read<quint8>());
	contact.age = data.read<quint16>(LittleEndian);
	debug() << "Contact found" << contact.uin << contact.nick << contact.firstName
			<< contact.lastName << contact.email << contact.authFlag << contact.status
			<< contact.gender << contact.age;	
	emit contactFound(contact);
	d->contacts.insert(contact.uin, contact);
	if (type == 0x01AE)
		close(true);
	return true;
}

MetaInfo::MetaInfo() :
	m_sequence(0)
{
	Q_ASSERT(!self);
	self = this;
	m_infos << SNACInfo(ExtensionsFamily, ExtensionsMetaSrvReply)
		<< SNACInfo(ExtensionsFamily, ExtensionsMetaError);
	connect(IcqProtocol::instance(), SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
			this, SLOT(onNewAccount(qutim_sdk_0_3::Account*)));
	foreach (Account *account, IcqProtocol::instance()->accounts())
		onNewAccount(account);
}

void MetaInfo::handleSNAC(AbstractConnection *conn, const SNAC &snac)
{
	Q_UNUSED(conn);
	if (snac.family() == ExtensionsFamily && snac.subtype() == ExtensionsMetaSrvReply) {
		TLVMap tlvs = snac.read<TLVMap>();
		if (tlvs.contains(0x01)) {
			DataUnit data(tlvs.value(0x01));
			data.skipData(6); // skip length field + my uin
			quint16 metaType = data.read<quint16>(LittleEndian);
			if (metaType == 0x07da) {
				quint16 reqNumber = data.read<quint16>(LittleEndian);
				QHash<quint16, AbstractMetaInfoRequest*>::iterator itr = m_requests.find(reqNumber);
				quint16 dataType = data.read<quint16>(LittleEndian);
				quint8 success = data.read<quint8>(LittleEndian);

				if (itr == m_requests.end()) {
					debug() << "Unexpected metainfo response" << reqNumber;
					return;
				}
				if (success == 0x0a) {
					if (!itr.value()->handleData(dataType, data))
						debug() << "Unexpected metainfo response with type" << hex << dataType;
				} else {
					debug() << "Meta request failed" << hex << success;
					itr.value()->close(false);
				}
			} else if (metaType == 0x07d0) {
				debug() << data.readAll().toHex();
			}
		}
	} else if (snac.family() == ExtensionsFamily && snac.subtype() == ExtensionsMetaError) {
		ProtocolError error(snac);
		debug() << QString("Error (%1, %2): %3")
				.arg(error.code(), 2, 16)
				.arg(error.subcode(), 2, 16)
				.arg(error.errorString());
	}
}

void MetaInfo::addRequest(AbstractMetaInfoRequest *request)
{
	m_requests.insert(request->id(), request);
}

bool MetaInfo::removeRequest(AbstractMetaInfoRequest *request)
{
	return m_requests.remove(request->id()) > 0;
}

void MetaInfo::onNewAccount(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
}

void MetaInfo::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	if (status == Status::Offline) {
		QHash<quint16, AbstractMetaInfoRequest*> requests = m_requests;
		foreach (AbstractMetaInfoRequest *req, requests)
			req->close(false);
		Q_ASSERT(m_requests.isEmpty());
	}
}

} } // namespace qutim_sdk_0_3::oscar
