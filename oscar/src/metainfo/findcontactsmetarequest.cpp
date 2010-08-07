/****************************************************************************
 *  findcontactmetarequest.cpp
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

#include "findcontactsmetarequest.h"
#include "tlvbasedmetarequest_p.h"
#include "metafields_p.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class FindContactsMetaRequestPrivate : public TlvBasedMetaRequestPrivate
{
public:
	QHash<QString, FindContactsMetaRequest::FoundContact> contacts;
};

FindContactsMetaRequest::FoundContact::FoundContact() :
	status(NonWebaware), authFlag(false), age(0)
{
}

FindContactsMetaRequest::FindContactsMetaRequest(IcqAccount *account, const MetaInfoValuesHash &values) :
	TlvBasedMetaRequest(account, new FindContactsMetaRequestPrivate)
{
	Q_D(FindContactsMetaRequest);
	d->values = values;
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
	contact.status = static_cast<Status>(data.read<quint16>(LittleEndian));
	contact.gender = genders().value(data.read<quint8>());
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

} } // namespace qutim_sdk_0_3::oscar
