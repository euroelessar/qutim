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

QDebug operator<<(QDebug dbg, const Category &cat)
{
	dbg.nospace() << "{" << cat.category << ", " << cat.keyword << "}";
	return dbg.space();
}

MetaInfo *MetaInfo::self = 0;

QString MetaInfoGenderField::toString() const
{
	return genders()->value(m_value);
}

QDebug &operator<<(QDebug &dbg, const MetaInfoGenderField &field)
{
	dbg << genders()->value(field.m_value);
	return dbg;
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
}

void AbstractMetaInfoRequest::close(bool ok)
{
	Q_D(AbstractMetaInfoRequest);
	d->ok = ok;
	if (MetaInfo::instance().removeRequest(this) || ok)
		emit done(ok);
}

QString ShortInfoMetaRequestPrivate::readString(const DataUnit &data)
{
	QString str = data.read<QString, quint16>(Util::asciiCodec(), LittleEndian);
	str.chop(1);
	return str;
}

void ShortInfoMetaRequestPrivate::readString(MetaInfoFieldEnum value, const DataUnit &data)
{
	QString str = readString(data);
	if (!str.isEmpty())
		values.insert(value, str);
}

void ShortInfoMetaRequestPrivate::readFlag(MetaInfoFieldEnum value, const DataUnit &data)
{
	bool f = static_cast<bool>(data.read<quint8>());
	if (f)
		values.insert(value, f);
}

void ShortInfoMetaRequestPrivate::dump()
{
	QHashIterator<MetaInfoField, QVariant> itr(values);
	while (itr.hasNext()) {
		itr.next();
		if (itr.value().canConvert<MetaInfoGenderField>())
			debug() << itr.key().toString() << itr.value().value<MetaInfoGenderField>();
		else if (itr.value().canConvert<CategoryList>())
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

ShortInfoMetaRequest::ValuesHash ShortInfoMetaRequest::values() const
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
		MetaInfoGenderField gender(data.read<quint8>());
		if (!gender.toString().isEmpty())
			d->values.insert(Gender, QVariant::fromValue(gender));
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
		category.keyword = readString(data);
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
	values.insert(Age, data.read<quint16>(LittleEndian));
	{
		MetaInfoGenderField gender(data.read<quint8>());
		if (!gender.toString().isEmpty())
			values.insert(Gender, QVariant::fromValue(gender));
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
		QString email = readString(data);
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
	if (snac.family() == ExtensionsFamily && snac.subtype() == ExtensionsMetaSrvReply) {
		TLVMap tlvs = snac.read<TLVMap>();
		if (tlvs.contains(0x01)) {
			DataUnit data(tlvs.value(0x01));
			data.skipData(6); // skip length field + my uin
			quint16 metaType = data.read<quint16>(LittleEndian);
			if (metaType == 0x07da) {
				quint16 reqNumber = data.read<quint16>(LittleEndian);
				QHash<quint16, AbstractMetaInfoRequest*>::iterator itr = m_requests.find(reqNumber);
				if (itr == m_requests.end()) {
					debug() << "Unexpected metainfo response" << reqNumber;
					return;
				}
				quint16 dataType = data.read<quint16>(LittleEndian);
				quint8 success = data.read<quint8>(LittleEndian);
				if (success == 0x0a) {
					if (!itr.value()->handleData(dataType, data))
						debug() << "Unexpected metainfo response with type" << hex << dataType;
				} else {
					debug() << "Meta request failed" << success;
				}
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
