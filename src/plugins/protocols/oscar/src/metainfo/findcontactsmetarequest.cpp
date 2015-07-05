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
		authFlag(false), status(NonWebaware), age(0)
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
	qDebug() << "Contact found" << contact.uin << contact.nick << contact.firstName
			<< contact.lastName << contact.email << contact.authFlag << contact.status
			<< contact.gender << contact.age;
	emit contactFound(contact);
	d->contacts.insert(contact.uin, contact);
	if (type == 0x01AE)
		close(true);
	return true;
}

} } // namespace qutim_sdk_0_3::oscar

