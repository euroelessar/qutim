/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "contactsearch_p.h"
#include "icqprotocol.h"
#include "icqaccount.h"
#include "icqcontact.h"
#include "oscarstatus.h"

namespace qutim_sdk_0_3 {

namespace oscar {

OscarContactSearch::OscarContactSearch(IcqAccount *account) :
	m_account(account)
{
}

DataItem OscarContactSearch::fields() const
{
	DataItem item;
	addField(item, Uin);
	addField(item, Nick);
	addField(item, Email);
	addField(item, FirstName);
	addField(item, LastName);
	addField(item, Gender);
	addField(item, AgeRange);
	//addField(item, HomeCountry);
	addField(item, HomeCity);
	//addField(item, Interests);
	addField(item, Languages);
	addField(item, OnlineFlag);
	return item;
}

void OscarContactSearch::addField(DataItem &item, const MetaField &field) const
{
	item.addSubitem(field.toDataItem(QVariant(), false));
}

void OscarContactSearch::start(const DataItem &fields)
{
	m_contacts.clear();
	Status::Type status = m_account->status().type();
	MetaInfoValuesHash values = MetaField::dataItemToHash(fields);
	if (status >= Status::Online && status <= Status::Invisible) {
		Q_ASSERT(!m_request);
		m_request.reset(new FindContactsMetaRequest(m_account, values));
		connect(m_request.data(), SIGNAL(contactFound(FindContactsMetaRequest::FoundContact)),
				SLOT(onNewContact(FindContactsMetaRequest::FoundContact)));
		connect(m_request.data(), SIGNAL(done(bool)), SLOT(onDone(bool)));
		m_request->send();
	} else {
		emit done(false);
	}
}

void OscarContactSearch::cancel()
{
	m_request.reset();
}

int OscarContactSearch::columnCount() const
{
	return 7;
}

QVariant OscarContactSearch::headerData(int column, int role)
{
	if (role != Qt::DisplayRole)
		return QVariant();
	switch(column) {
	case 0:
		return QT_TRANSLATE_NOOP("ContactSearch", "UIN").toString();
	case 1:
		return QT_TRANSLATE_NOOP("ContactSearch", "Nick").toString();
	case 2:
		return QT_TRANSLATE_NOOP("ContactSearch", "First name").toString();
	case 3:
		return QT_TRANSLATE_NOOP("ContactSearch", "Last name").toString();
	case 4:
		return QT_TRANSLATE_NOOP("ContactSearch", "Email").toString();
	case 5:
		return QT_TRANSLATE_NOOP("ContactSearch", "Gender").toString();
	case 6:
		return QT_TRANSLATE_NOOP("ContactSearch", "Age").toString();
	}
	return QVariant();
}

int OscarContactSearch::rowCount() const
{
	return m_contacts.count();
}

QVariant OscarContactSearch::data(int row, int column, int role)
{
	if (role != Qt::DisplayRole && role != Qt::DecorationRole)
		return QVariant();
	FindContactsMetaRequest::FoundContact contact = m_contacts.value(row);
	if (role == Qt::DecorationRole) {
		if (column == 0) {
			switch (contact.status) {
			case FindContactsMetaRequest::Online:
				return OscarStatus(OscarOnline).icon();
			case FindContactsMetaRequest::Offline:
				return OscarStatus(OscarOffline).icon();
			case FindContactsMetaRequest::NonWebaware:
				return OscarStatus(OscarInvisible).icon();
			}
		}
		return QVariant();
	}
	if (role == Qt::DisplayRole) {
		switch (column) {
		case 0:
			return contact.uin;
		case 1:
			return contact.nick;
		case 2:
			return contact.firstName;
		case 3:
			return contact.lastName;
		case 4:
			return contact.email;
		case 5:
			return contact.gender;
		case 6:
			return contact.age > 0 ? contact.age : QVariant();
		}
	}
	return QVariant();
}

Contact *OscarContactSearch::contact(int row)
{
	FindContactsMetaRequest::FoundContact contact = m_contacts.value(row);
	if (!contact.uin.isEmpty())
		return m_account->getContact(contact.uin, true);
	return 0;
}

void OscarContactSearch::onNewContact(const FindContactsMetaRequest::FoundContact &contact)
{
	int row = m_contacts.count();
	emit rowAboutToBeAdded(row);
	m_contacts.push_back(contact);
	emit rowAdded(row);
}

void OscarContactSearch::onDone(bool ok)
{
	m_request.reset();
	emit done(ok);
}

OscarContactSearchFactory::OscarContactSearchFactory() :
	GeneralContactSearchFactory(IcqProtocol::instance())
{
}

ContactSearchRequest *OscarContactSearchFactory::request(const QString &name) const
{
	IcqAccount *acc = qobject_cast<IcqAccount*>(account(name));
	if (!acc)
		return 0;
	return new OscarContactSearch(acc);
}

} } // namespace qutim_sdk_0_3::oscar

