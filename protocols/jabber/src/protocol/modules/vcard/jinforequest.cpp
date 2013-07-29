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
#include "jinforequest.h"
#include "jvcardmanager.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/account.h>
#include <jreen/vcard.h>
#include <QUrl>
#include <QFile>
#include <QDate>
#include <QImageReader>

namespace Jabber
{
static QStringList init_names()
{
    return QStringList()<<
        QStringLiteral("nick")<<
        QStringLiteral("firstName")<<
        QStringLiteral("middleName")<<
        QStringLiteral("lastName")<<
        QStringLiteral("birthday")<<
        QStringLiteral("homepage")<<
        QStringLiteral("homePhone")<<
        QStringLiteral("workPhone")<<
        QStringLiteral("mobilePhone")<<
        QStringLiteral("phone")<<
        QStringLiteral("personalEmail")<<
        QStringLiteral("workEmail")<<
        QStringLiteral("email")<<
        QStringLiteral("homeAddress")<<
        QStringLiteral("workAddress")<<
        QStringLiteral("address")<<
        QStringLiteral("country")<<
        QStringLiteral("region")<<
        QStringLiteral("city")<<
        QStringLiteral("postcode")<<
        QStringLiteral("street")<<
        QStringLiteral("extendedAddress")<<
        QStringLiteral("postbox")<<
        QStringLiteral("orgName")<<
        QStringLiteral("orgUnit")<<
        QStringLiteral("title")<<
        QStringLiteral("role")<<
        QStringLiteral("about")<<
        QStringLiteral("features");

//	for (int i = 0, size = sizeof(cnames)/sizeof(char*); i < size; i++)
//		names << QLatin1String(cnames[i]);
}

Q_GLOBAL_STATIC_WITH_ARGS(QStringList, names, (init_names()))

static QList<LocalizedString> init_titles()
{

    QList<LocalizedString> titles;
    titles << QT_TRANSLATE_NOOP("ContactInfo", "Nickname")
           << QT_TRANSLATE_NOOP("ContactInfo", "First name")
           << QT_TRANSLATE_NOOP("ContactInfo", "Middle name")
           << QT_TRANSLATE_NOOP("ContactInfo", "Last name")
           << QT_TRANSLATE_NOOP("ContactInfo", "Birthday")
           << QT_TRANSLATE_NOOP("ContactInfo", "Homepage")
           << QT_TRANSLATE_NOOP("ContactInfo", "Home phone")
           << QT_TRANSLATE_NOOP("ContactInfo", "Work phone")
           << QT_TRANSLATE_NOOP("ContactInfo", "Mobile phone")
           << QT_TRANSLATE_NOOP("ContactInfo", "Phone")
           << QT_TRANSLATE_NOOP("ContactInfo", "Personal email")
           << QT_TRANSLATE_NOOP("ContactInfo", "Work email")
           << QT_TRANSLATE_NOOP("ContactInfo", "Email")
           << QT_TRANSLATE_NOOP("ContactInfo", "Home address")
           << QT_TRANSLATE_NOOP("ContactInfo", "Work address")
           << QT_TRANSLATE_NOOP("ContactInfo", "Address")
           << QT_TRANSLATE_NOOP("ContactInfo", "Country")
           << QT_TRANSLATE_NOOP("ContactInfo", "Region")
           << QT_TRANSLATE_NOOP("ContactInfo", "City")
           << QT_TRANSLATE_NOOP("ContactInfo", "Postcode")
           << QT_TRANSLATE_NOOP("ContactInfo", "Street")
           << QT_TRANSLATE_NOOP("ContactInfo", "Address")
           << QT_TRANSLATE_NOOP("ContactInfo", "Postbox")
           << QT_TRANSLATE_NOOP("ContactInfo", "Company")
           << QT_TRANSLATE_NOOP("ContactInfo", "Department")
           << QT_TRANSLATE_NOOP("ContactInfo", "Position")
           << QT_TRANSLATE_NOOP("ContactInfo", "Role")
           << QT_TRANSLATE_NOOP("ContactInfo", "About");
    return titles;
}

Q_GLOBAL_STATIC_WITH_ARGS(QList<LocalizedString>, titles, (init_titles()))

class JInfoRequestPrivate
{
public:
	Jreen::JID jid;
	Jreen::VCard::Ptr vcard;
	Jreen::VCardManager *manager;
};

JInfoRequest::JInfoRequest(Jreen::VCardManager *manager, QObject *object)
	: InfoRequest(object), d_ptr(new JInfoRequestPrivate)
{
	Q_D(JInfoRequest);
	d->jid = object->property("id").toString();
	d->manager = manager;
}

JInfoRequest::~JInfoRequest()
{
}

void JInfoRequest::setFetchedVCard(const Jreen::VCard::Ptr &vcard)
{
	Q_D(JInfoRequest);
	d->vcard = vcard;
	if (state() == InfoRequest::Requesting)
		setState(InfoRequest::RequestDone);
}

void JInfoRequest::doRequest(const QSet<QString> &hints)
{
	Q_D(JInfoRequest);
	Q_UNUSED(hints);
	Jreen::VCardReply *reply = d->manager->fetch(d->jid);
	connect(reply, SIGNAL(vCardFetched(Jreen::VCard::Ptr,Jreen::JID)),
	        SLOT(setFetchedVCard(Jreen::VCard::Ptr)));
	setState(InfoRequest::Requesting);
}

void JInfoRequest::doUpdate(const DataItem &dataItem)
{
	Q_D(JInfoRequest);
	d->vcard = convert(dataItem);
	Jreen::VCardReply *reply = d->manager->store(d->vcard);
	connect(reply, SIGNAL(finished()),
	        SLOT(onStoreFinished()));
	setState(Updating);
}

void JInfoRequest::doCancel()
{
	setState(Canceled);
}

DataItem JInfoRequest::createDataItem() const
{
	Q_D(const JInfoRequest);
	DataItem item;
	const bool isAccount = qobject_cast<Account*>(object());
	{
		DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
		// General page
		{
			//// Avatar
			{
				QString photoPath;
				JVCardManager::ensurePhoto(d->vcard->photo(), &photoPath);
				DataItem avatarItem(QLatin1String("avatar"),
									QT_TRANSLATE_NOOP("ContactInfo", "Avatar"),
									QPixmap(photoPath));
				avatarItem.setProperty("imagePath", photoPath);
				avatarItem.setProperty("hideTitle", true);
				avatarItem.setProperty("imageSize", QSize(64, 64));
				avatarItem.setProperty("defaultImage", Icon(QLatin1String("qutim")).pixmap(64));
				general.addSubitem(avatarItem);
			}
			// name
//			QString name = d->vcard->nickname().isEmpty() ? d->vcard->formattedName() : d->vcard->nickname();
			addItemList(Nick, general, d->vcard->nickname());
			addItemList(FirstName, general, d->vcard->name().given());
			addItemList(MiddleName, general, d->vcard->name().middle());
			addItemList(LastName, general, d->vcard->name().family());
			// birthday
			addItem(Birthday, general, d->vcard->birthday().date());
			//// homepage
			addItem(Homepage, general, QVariant::fromValue(d->vcard->url()));
		}
		if (!isAccount) {
		//// telephone
			if (!d->vcard->telephones().empty()) {
				foreach (Jreen::VCard::Telephone phone, d->vcard->telephones())
					addItem(getPhoneType(phone), general, phone.number());
			} else {
				addItem(Phone, general, QString());
			}
		//// email
			if (!d->vcard->emails().empty()) {
				foreach (const Jreen::VCard::EMail &email, d->vcard->emails())
					addItem(getEmailType(email), general,email.userId());
			} else {
				addItem(Email, general, QString());
			}
		}
		item.addSubitem(general);
	}
	if (isAccount) {
	// Telephone page
		DataItem phoneGroup(QLatin1String("phoneGroup"),
							QT_TRANSLATE_NOOP("ContactInfo", "Phones"),
							QVariant());
		phoneGroup.allowModifySubitems(telephoneItem(Jreen::VCard::Telephone()));
		foreach (const Jreen::VCard::Telephone &phone, d->vcard->telephones())
			phoneGroup << telephoneItem(phone);
		item.addSubitem(phoneGroup);

	// Email page
		DataItem emailGroup(QLatin1String("emailGroup"),
							QT_TRANSLATE_NOOP("ContactInfo", "E-mails"),
							QVariant());
		emailGroup.allowModifySubitems(emailItem(Jreen::VCard::EMail()));
		foreach (const Jreen::VCard::EMail &email, d->vcard->emails())
			emailGroup << emailItem(email);
		item.addSubitem(emailGroup);
	}
	// Addresses page
	{
		DataItem addresses(QLatin1String("addressGroup"),
						   QT_TRANSLATE_NOOP("ContactInfo", "Addresses"),
						   QVariant());
		if (isAccount)
			addresses.allowModifySubitems(addressItem(Jreen::VCard::Address()));
		if (isAccount || !d->vcard->addresses().empty()) {
			foreach (const Jreen::VCard::Address &address, d->vcard->addresses())
				addresses.addSubitem(addressItem(address));
		} else {
			addItem(Address, addresses, QString());
		}
		item.addSubitem(addresses);
	}
	// Work page
	{
		DataItem work(QT_TRANSLATE_NOOP("ContactInfo", "Work"));
		addItem(OrgName, work, d->vcard->organization().name());
		addItem(OrgUnit, work, d->vcard->organization().units().value(0));
		addItem(Title, work, d->vcard->title());
		addItem(Role, work, d->vcard->role());
		item.addSubitem(work);
	}
	// About page
	{
		DataItem about(QT_TRANSLATE_NOOP("ContactInfo", "About"));
		addMultilineItem(About, about, d->vcard->desc());
		item.addSubitem(about);
	}

	// Features page
	//JContactResource *c = qobject_cast<JContactResource*>(d->account->getUnit(d->contact));
	//qDebug() << "get features from" << d->contact << d->account->getUnit(d->contact) << c;
	//if(c) {
	//	debug() << c->features();
	//	DataItem features(QT_TRANSLATE_NOOP("ContactInfo", "Features"));
	//	addItemList(Features, features,c->features().toList());
	//	item.addSubitem(features);
	//}
	return item;
}

template<typename T>
T getItem(const DataItem &item, JInfoRequest::DataType type)
{
	return item.subitem(names()->at(type), true).data<T>();
}

template<typename T>
T getItem(const DataItem &item, const char *type)
{
	return item.subitem(QLatin1String(type), true).data<T>();
}

template<typename T>
void setType(T &val, const DataItem &item, const char *field)
{
	int type = item.subitem(QLatin1String(field), true).property("identificator", 0);
	val.setType(static_cast<typename T::Type>(type), true);
}

Jreen::VCard::Ptr JInfoRequest::convert(const DataItem &item) const
{
	Account *account = qobject_cast<Account*>(object());
	Q_ASSERT(account);
	Jreen::VCard::Ptr vcard = Jreen::VCard::Ptr::create();
	{
		DataItem avatarItem = item.subitem(QLatin1String("avatar"));
		QString path = avatarItem.isNull()
		        ? account->property("avatar").toString()
		        : avatarItem.property("imagePath", QString());

		QByteArray avatarData;
		QString mimeType;
		QFile file(path);
		if (file.open(QFile::ReadOnly)) {
			avatarData = file.readAll();
			QImageReader reader(path);
			QByteArray format = reader.format().toLower();
			if (format == "jpg")
				format = "jpeg";
			else if (format == "svg")
				format = "svg+xml";
			if (!format.isEmpty())
				mimeType = QLatin1String("image/" + format);
		}

		Jreen::VCard::Photo photo;
		photo.setData(avatarData, mimeType);
		vcard->setPhoto(photo);
	}
	vcard->setNickname(getItem<QString>(item, Nick));
	vcard->setName(getItem<QString>(item, LastName),
				   getItem<QString>(item, FirstName),
				   getItem<QString>(item, MiddleName));
	vcard->setBirthday(getItem<QDate>(item, Birthday));
	vcard->setUrl(QUrl(getItem<QString>(item, Homepage)));
	vcard->setDesc(getItem<QString>(item, About));
	QStringList units;
	QString unit = getItem<QString>(item, OrgUnit);
	if (!unit.isEmpty())
		units << unit;
	vcard->setOrganization(getItem<QString>(item, OrgName), units);
	vcard->setTitle(getItem<QString>(item, Title));
	vcard->setRole(getItem<QString>(item, Role));

	DataItem phones = item.subitem(QLatin1String("phoneGroup"), true);
	foreach (const DataItem &phoneItem, phones.subitems()) {
		Jreen::VCard::Telephone phone;
		QString number = getItem<QString>(phoneItem, "number");
		if (number.isEmpty())
			continue;
		phone.setNumber(number);
		setType(phone, item, "phoneTypes");
		vcard->addTelephone(phone);
	}

	DataItem emails = item.subitem(QLatin1String("emailGroup"), true);
	foreach (const DataItem &emailItem, emails.subitems()) {
		Jreen::VCard::EMail email;
		QString userId = getItem<QString>(emailItem, "email");
		if (userId.isEmpty())
			continue;
		email.setUserId(userId);
		setType(email, item, "emailTypes");
		vcard->addEmail(email);
	}

	DataItem addresses = item.subitem(QLatin1String("addressGroup"), true);
	foreach (const DataItem &addrItem, addresses.subitems()) {
		Jreen::VCard::Address addr;
		addr.setCountry(getItem<QString>(addrItem, Country));
		addr.setRegion(getItem<QString>(addrItem, Region));
		addr.setLocality(getItem<QString>(addrItem, City));
		addr.setPostCode(getItem<QString>(addrItem, Postcode));
		addr.setStreet(getItem<QString>(addrItem, Street));
		addr.setExtendedAddress(getItem<QString>(addrItem, ExtendedAddress));
		addr.setPostBox(getItem<QString>(addrItem, Postbox));
		setType(addr, item, "addressTypes");
		vcard->addAdress(addr);
	}

	return vcard;
}

template <typename T>
DataItem typeItem(const T &val, const char *name,
				  const QList<LocalizedString> &descriptions,
				  QList<typename T::Type> types)
{
	int current = -1;
	QVariantList ids;
	for (int i = 0, n = types.count(); i < n; ++i) {
		typename T::Type type = types.at(i);
		if (current == -1 && val.testType(type))
			current = i;
		ids << static_cast<int>(type);
	}

	DataItem item(QLatin1String(name),
				  QT_TRANSLATE_NOOP("ContactInfo", "Type"),
				  qVariantFromValue(descriptions.value(current)));
	item.setProperty("alternatives", qVariantFromValue(descriptions));
	item.setProperty("identificators", ids);
	item.setProperty("additional", true);
	return item;
}

DataItem JInfoRequest::telephoneItem(const Jreen::VCard::Telephone &phone)
{
	static QList<LocalizedString> descriptions = QList<LocalizedString>()
			<< QT_TRANSLATE_NOOP("ContactInfo", "Home")
			<< QT_TRANSLATE_NOOP("ContactInfo", "Work")
			<< QT_TRANSLATE_NOOP("ContactInfo", "Cell");

	static QList<Jreen::VCard::Telephone::Type> types = QList<Jreen::VCard::Telephone::Type>()
			<< Jreen::VCard::Telephone::Home
			<< Jreen::VCard::Telephone::Work
			<< Jreen::VCard::Telephone::Cell;

	DataItem item(titles()->at(getPhoneType(phone)));
	item.setProperty("hideTitle", true);
	item << DataItem(QLatin1String("number"),
					 QT_TRANSLATE_NOOP("ContactInfo", "Number"),
					 phone.number());
	item << typeItem(phone, "phoneTypes", descriptions, types);
	return item;
}

DataItem JInfoRequest::emailItem(const Jreen::VCard::EMail &email)
{
	static QList<LocalizedString> descriptions = QList<LocalizedString>()
			<< QT_TRANSLATE_NOOP("ContactInfo", "Home")
			<< QT_TRANSLATE_NOOP("ContactInfo", "Work");

	static QList<Jreen::VCard::EMail::Type> types = QList<Jreen::VCard::EMail::Type>()
			<< Jreen::VCard::EMail::Home
			<< Jreen::VCard::EMail::Work;

	DataItem item(titles()->at(getEmailType(email)));
	item.setProperty("hideTitle", true);
	item << DataItem(QLatin1String("email"),
					 QT_TRANSLATE_NOOP("ContactInfo", "E-mail"),
					 email.userId());
	item << typeItem(email, "emailTypes", descriptions, types);
	return item;
}

DataItem JInfoRequest::addressItem(const Jreen::VCard::Address &address) const
{
	static QList<LocalizedString> descriptions = QList<LocalizedString>()
			<< QT_TRANSLATE_NOOP("ContactInfo", "Home")
			<< QT_TRANSLATE_NOOP("ContactInfo", "Work");

	static QList<Jreen::VCard::Address::Type> types = QList<Jreen::VCard::Address::Type>()
			<< Jreen::VCard::Address::Home
			<< Jreen::VCard::Address::Work;

	DataItem addrItem(titles()->at(getAddressType(address)));
	if (qobject_cast<Account*>(object())) {
		addrItem.setProperty("hideTitle", true);
		addrItem << typeItem(address, "addressTypes", descriptions, types);
	}
	addItem(Country, addrItem, address.country());
	addItem(Region, addrItem, address.region());
	addItem(City, addrItem, address.locality());
	addItem(Postcode, addrItem, address.postCode());
	addItem(Street, addrItem, address.street());
	addItem(ExtendedAddress, addrItem, address.extendedAddress());
	addItem(Postbox, addrItem, address.postBox());
	return addrItem;
}

JInfoRequest::DataType JInfoRequest::getPhoneType(const Jreen::VCard::Telephone &phone)
{
	if (phone.testType(Jreen::VCard::Telephone::Home))
		return HomePhone;
	else if (phone.testType(Jreen::VCard::Telephone::Work))
		return WorkPhone;
	else if (phone.testType(Jreen::VCard::Telephone::Cell))
		return MobilePhone;
	return Phone;
}

JInfoRequest::DataType JInfoRequest::getEmailType(const Jreen::VCard::EMail &email)
{
	if (email.testType(Jreen::VCard::EMail::Home))
		return PersonalEmail;
	else if (email.testType(Jreen::VCard::EMail::Work))
		return WorkEmail;
	return Email;
}

JInfoRequest::DataType JInfoRequest::getAddressType(const Jreen::VCard::Address &address)
{
	if (address.testType(Jreen::VCard::Address::Home))
		return HomeAddress;
	else if (address.testType(Jreen::VCard::Address::Work))
		return WorkAddress;
	return Address;
}

void JInfoRequest::addItem(DataType type, DataItem &group, const QVariant &data) const
{
	DataItem item(names()->at(type), titles()->at(type), data);

	group.addSubitem(item);
}

void JInfoRequest::addMultilineItem(DataType type, DataItem &group, const QString &data) const
{
	DataItem item(names()->at(type), titles()->at(type), data);
	item.setProperty("multiline", true);
	item.setProperty("hideTitle", true);
	group.addSubitem(item);
}

void JInfoRequest::addItemList(DataType type, DataItem &group, const QString &data) const
{
	addItem(type, group, qobject_cast<Account*>(object()) ? data : QVariant(data.split(',', QString::SkipEmptyParts)));
}

void JInfoRequest::addItemList(DataType type, DataItem &group, const QStringList &data) const
{
	addItem(type, group, data);
}

void JInfoRequest::onStoreFinished()
{
	Jreen::VCardReply *reply = qobject_cast<Jreen::VCardReply*>(sender());
	Q_ASSERT(reply);
	if (reply->error())
		setState(Error);
	else
		setState(Updated);
}


}

