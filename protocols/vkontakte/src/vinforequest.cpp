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

#include "vinforequest.h"
#include "vaccount.h"
#include "vcontact.h"
#include <qutim/json.h>
#include <QNetworkReply>
#include <QDate>
#include <QDebug>
#include <QCache>
#include <vreen/connection.h>

using namespace qutim_sdk_0_3;

void init_names(QStringList &names)
{
	const char *cnames[] = {
		"nick",
		"firstName",
		"lastName",
		"gender",
		"birthday",
		"city",
		"country",
		"photo",
		"homePhone",
		"mobilePhone",
		"university",
		"faculty",
		"graduation"
	};
	for (int i = 0, size = sizeof(cnames)/sizeof(char*); i < size; i++)
		names << QLatin1String(cnames[i]);
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QStringList, names, init_names(*x))

void init_titles(QList<LocalizedString> &titles)
{
	titles << QT_TRANSLATE_NOOP("ContactInfo", "Nickname")
		   << QT_TRANSLATE_NOOP("ContactInfo", "First name")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Last name")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Gender")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Birthday")
		   << QT_TRANSLATE_NOOP("ContactInfo", "City")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Country")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Photo")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Home phone")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Mobile phone")
		   << QT_TRANSLATE_NOOP("ContactInfo", "University")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Faculty")
		   << QT_TRANSLATE_NOOP("ContactInfo", "Graduation year");
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QList<LocalizedString>, titles, init_titles(*x))

void init_genders(QList<LocalizedString> &genders)
{
	genders << QT_TRANSLATE_NOOP("ContactInfo", "Undefined")
			<< QT_TRANSLATE_NOOP("ContactInfo", "Female")
			<< QT_TRANSLATE_NOOP("ContactInfo", "Male");
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QList<LocalizedString>, genders, init_genders(*x))

typedef QCache<int, QString> NameMapper;

Q_GLOBAL_STATIC(NameMapper, cities)
Q_GLOBAL_STATIC(NameMapper, countries)

static bool isStatusOnline(const Status &status)
{
	Status::Type type = status.type();
	return type != Status::Offline && type != Status::Connecting;
}

VInfoFactory::VInfoFactory(VAccount *account) :
	QObject(account), m_account(account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status, qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status, qutim_sdk_0_3::Status)));
}

VInfoFactory::SupportLevel VInfoFactory::supportLevel(QObject *object)
{
	SupportLevel level = NotSupported;
	if (m_account == object) {
		level = ReadOnly;
	} else if (VContact *contact = qobject_cast<VContact*>(object)) {
		if (contact->account() == m_account)
			level = ReadOnly;
	}
	if (level == ReadOnly && !isStatusOnline(m_account->status()))
		level = Unavailable;
	return level;
}

InfoRequest *VInfoFactory::createrDataFormRequest(QObject *object)
{
	if (m_account == object) {
		return new VInfoRequest(m_account);
	} else if (VContact *contact = qobject_cast<VContact*>(object)) {
		if (contact->account() == m_account)
			return new VInfoRequest(contact);
	}
	return 0;
}

bool VInfoFactory::startObserve(QObject *object)
{
	if (m_account == object) {
		return true;
	} else if (VContact *contact = qobject_cast<VContact*>(object)) {
		if (contact->account() == m_account) {
			m_contacts.insert(contact);
			return true;
		}
	}
	return false;
}

bool VInfoFactory::stopObserve(QObject *object)
{
	if (m_account == object) {
		return true;
	} else if (VContact *contact = qobject_cast<VContact*>(object)) {
		if (contact->account() == m_account)
			return m_contacts.remove(contact) != 0;
	}
	return false;
}

void VInfoFactory::onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
										  const qutim_sdk_0_3::Status &previous)
{
	bool isOnline = isStatusOnline(status);
	bool wasOnline = isStatusOnline(previous);
	bool supported;

	if (isOnline && !wasOnline)
		supported = true;
	else if (!isOnline && wasOnline)
		supported = false;
	else
		return;

	SupportLevel level = supported ? ReadOnly : Unavailable;
	setSupportLevel(m_account, level);
	foreach (ChatUnit *unit, m_contacts)
		setSupportLevel(unit, level);
}

VInfoRequest::VInfoRequest(QObject *parent) :
	InfoRequest(parent),
	m_unknownCount(0)
{
	if (VAccount *account = qobject_cast<VAccount*>(parent)) {
		m_id = QString::number(account->uid());
		m_connection = account->connection();
	} else if (VContact *contact = qobject_cast<VContact*>(parent)) {
		m_id = contact->id();
		m_connection = static_cast<VAccount*>(contact->account())->connection();
	}
	Q_ASSERT(m_connection);
}

DataItem VInfoRequest::createDataItem() const
{
	DataItem item;
	// General page
	{
		DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
		{
			DataItem person(QT_TRANSLATE_NOOP("ContactInfo", "Person"));
			addItem(NickName, person, "nickname");
			addItem(FirstName, person, "first_name");
			addItem(LastName, person, "last_name");
			addItem(Sex, person, genders()->value(m_data.value("sex", 0).toInt()).toString());
			QString bdate = m_data.value("bdate").toString();
			QDate birthday;
			if (bdate.count('.') == 2)
				birthday = QDate::fromString(bdate, "d.M.yyyy");
			else if (bdate.count('.') == 1)
				birthday = QDate::fromString(bdate, "d.M");
			addItem(BDate, person, birthday);
			general.addSubitem(person);
		}
		{
			DataItem address(QT_TRANSLATE_NOOP("ContactInfo", "Home address"));
			addItem(Country, address, "country");
			addItem(City, address, "city");
			addItem(HomePhone, address, "home_phone");
			addItem(MobilePhone, address, "mobile_phone");
			general.addSubitem(address);
		}
		{
			DataItem education(QT_TRANSLATE_NOOP("ContactInfo", "Education"));
			addItem(University, education, "university_name");
			addItem(Faculty, education, "faculty_name");
			addItem(Graduation, education, "graduation");
			general.addSubitem(education);
		}
		item.addSubitem(general);
	}
	return item;
}

void VInfoRequest::doRequest(const QSet<QString> &hints)
{
	Q_UNUSED(hints);
	QVariantMap data;
	data.insert("uids", m_id);
	data.insert("fields",
				"uid,first_name,last_name,nickname,sex,bdate,city,"
				"country,photo_medium,has_mobile,contacts,education");
	QNetworkReply *reply = m_connection->request("getProfiles", data);
	connect(this, SIGNAL(canceled()), reply, SLOT(deleteLater()));
	connect(reply, SIGNAL(finished()), this, SLOT(onRequestFinished()));
	setState(InfoRequest::Requesting);
}

void VInfoRequest::doUpdate(const DataItem &dataItem)
{
	Q_UNUSED(dataItem);
	Q_ASSERT(!"The method is not implemented");
}

void VInfoRequest::doCancel()
{
	emit canceled();
	setState(InfoRequest::Canceled);
}

void VInfoRequest::onRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();
	debug() << Q_FUNC_INFO << rawData;
	m_data = Json::parse(rawData).toMap().value("response").toList().value(0).toMap();
	ensureAddress(Country);
	ensureAddress(City);
	if (m_unknownCount == 0)
		setState(InfoRequest::RequestDone);
}

struct FuncPointerHelper
{
	NameMapper *mapper;
};

void VInfoRequest::onAddressEnsured()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QString field = reply->property("field").toString();
	qptrdiff tmp = reply->property("mapper").value<qptrdiff>();
	FuncPointerHelper *helper = reinterpret_cast<FuncPointerHelper*>(tmp);
	QByteArray rawData = reply->readAll();
	debug() << Q_FUNC_INFO << rawData;
	QVariantMap data = Json::parse(rawData).toMap().value("response").toList().value(0).toMap();
	QString name = data.value("name").toString();
	m_data.insert(field, name);
	helper->mapper->insert(data.value("cid").toInt(), new QString(name));
	delete helper;
	m_unknownCount--;
	if (m_unknownCount == 0)
		setState(InfoRequest::RequestDone);
}

void VInfoRequest::ensureAddress(DataType type)
{
	QString method;
	QString field;
	NameMapper *mapper;
	switch (type) {
	case Country:
		method = QLatin1String("getCountries");
		field = QLatin1String("country");
		mapper = countries();
		break;
	case City:
		method = QLatin1String("getCities");
		field = QLatin1String("city");
		mapper = cities();
		break;
	default:
		return;
	}
	int id = m_data.value(field, -1).toInt();
	if (id == -1)
		return;
	if (QString *obj = mapper->object(id)) {
		m_data.insert(field, *obj);
	} else {
		QVariantMap data;
		data.insert(QLatin1String("cids"), QString::number(id));
		FuncPointerHelper *helper = new FuncPointerHelper;
		helper->mapper = mapper;
		QNetworkReply *reply = m_connection->request(method, data);
		reply->setProperty("field", field);
		reply->setProperty("mapper", reinterpret_cast<qptrdiff>(helper));
		connect(this, SIGNAL(canceled()), reply, SLOT(deleteLater()));
		connect(reply, SIGNAL(finished()), this, SLOT(onAddressEnsured()));
		m_unknownCount++;
	}
}

void VInfoRequest::addItem(DataType type, DataItem &group, const QVariant &data) const
{
	DataItem item(names()->at(type), titles()->at(type), data);
	if (type == BDate && data.canConvert<QDate>()
			&& data.value<QDate>().year() == 1900)
		item.setProperty("hideYear", true);
	group.addSubitem(item);
}

