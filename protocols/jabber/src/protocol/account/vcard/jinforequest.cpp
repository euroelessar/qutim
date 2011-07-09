#include "jinforequest.h"
#include "jvcardmanager.h"
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <jreen/vcard.h>
#include <QUrl>
#include <QFile>
#include <QDate>
#include "../roster/jcontactresource.h"
#include "../jaccount.h"

namespace Jabber
{
void init_names(QStringList &names)
{
	const char *cnames[] = {
		"nick",
		"firstName",
		"middleName",
		"lastName",
		"birthday",
		"homepage",
		"homePhone",
		"workPhone",
		"mobilePhone",
		"phone",
		"personalEmail",
		"workEmail",
		"email",
		"homeAddress",
		"workAddress",
		"address",
		"country",
		"region",
		"city",
		"postcode",
		"street",
		"extendedAddress",
		"postbox",
		"orgName",
		"orgUnit",
		"title",
		"role",
		"about",
		"features"
	};
	for (int i = 0, size = sizeof(cnames)/sizeof(char*); i < size; i++)
		names << QLatin1String(cnames[i]);
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QStringList, names, init_names(*x))

void init_titles(QList<LocalizedString> &titles)
{
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
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QList<LocalizedString>, titles, init_titles(*x))

class JInfoRequestPrivate
{
public:
	Jreen::VCard::Ptr vcard;
	JVCardManager *manager;
	bool accountInfo;
};

JInfoRequest::JInfoRequest(JVCardManager *manager, QObject *object)
	: InfoRequest(object), d_ptr(new JInfoRequestPrivate)
{
	Q_D(JInfoRequest);
	d->manager = manager;
	d->accountInfo = d->manager->account() == object;
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
	d->manager->fetchVCard(object()->property("id").toString(), this);
	setState(InfoRequest::Requesting);
}

void JInfoRequest::doUpdate(const DataItem &dataItem)
{
	Q_D(JInfoRequest);
	d->vcard = convert(dataItem);
	d->manager->storeVCard(d->vcard, this, SLOT(onIqReceived(Jreen::IQ,int)));
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
	{
		DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
		// General page
		{
			//// Avatar
			{
				DataItem avatarItem(QLatin1String("avatar"),
									QT_TRANSLATE_NOOP("ContactInfo", "Avatar"),
									QPixmap(object()->property("avatar").toString()));
				avatarItem.setProperty("hideTitle", true);
				avatarItem.setProperty("imageSize", QSize(64, 64));
				avatarItem.setProperty("defaultImage", Icon(QLatin1String("qutim")).pixmap(64));
				general.addSubitem(avatarItem);
			}
			// name
			QString name = d->vcard->nickname().isEmpty() ? d->vcard->nickname() : d->vcard->formattedName();
			addItemList(Nick, general,name);
			addItemList(FirstName, general, d->vcard->name().given());
			addItemList(MiddleName, general, d->vcard->name().middle());
			addItemList(LastName, general, d->vcard->name().family());
			// birthday
			addItem(Birthday, general, d->vcard->bday().date());
			//// homepage
			addItemList(Homepage, general, d->vcard->url().toString());
		}
		//// telephone
		if (!d->accountInfo) {
			if (!d->vcard->telephones().empty()) {
				foreach (Jreen::VCard::Telephone phone, d->vcard->telephones())
					addItem(getPhoneType(phone), general, phone.number());
			} else {
				addItem(Phone, general, QString());
			}
		}
		//// email
		if (!d->accountInfo) {
			if (!d->vcard->emails().empty()) {
				foreach (const Jreen::VCard::EMail &email, d->vcard->emails())
					addItem(getEmailType(email), general,email.userId());
			} else {
				addItem(Email, general, QString());
			}
		}
		item.addSubitem(general);
	}
	// Telephone page
	if (d->accountInfo) {
		DataItem phoneGroup(QLatin1String("phoneGroup"),
							QT_TRANSLATE_NOOP("ContactInfo", "Phones"),
							QVariant());
		phoneGroup.allowModifySubitems(telephoneItem(Jreen::VCard::Telephone()));
		foreach (const Jreen::VCard::Telephone &phone, d->vcard->telephones())
			phoneGroup << telephoneItem(phone);
		item.addSubitem(phoneGroup);
	}
	// Email page
	if (d->accountInfo) {
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
		if (d->accountInfo)
			addresses.allowModifySubitems(addressItem(Jreen::VCard::Address()));
		if (d->accountInfo || !d->vcard->addresses().empty()) {
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
	Q_D(const JInfoRequest);
	Q_UNUSED(item);
	Jreen::VCard::Ptr vcard = Jreen::VCard::Ptr::create();
	{
		JAccount *acc = d->manager->account();
		DataItem avatarItem = item.subitem(QLatin1String("avatar"));
		QString path = avatarItem.isNull() ?
							acc->avatar() :
							avatarItem.property("imagePath", QString());

		QByteArray avatarData;
		QFile file(path);
		if (file.open(QFile::ReadOnly))
			avatarData = file.readAll();

		Jreen::VCard::Photo photo;
		photo.setData(avatarData);
		vcard->setPhoto(photo);
	}
	vcard->setNickname(getItem<QString>(item, Nick));
	vcard->setName(getItem<QString>(item, LastName),
				   getItem<QString>(item, FirstName),
				   getItem<QString>(item, MiddleName));
	vcard->setBday(getItem<QDateTime>(item, Birthday));
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
	Q_D(const JInfoRequest);
	static QList<LocalizedString> descriptions = QList<LocalizedString>()
			<< QT_TRANSLATE_NOOP("ContactInfo", "Home")
			<< QT_TRANSLATE_NOOP("ContactInfo", "Work");

	static QList<Jreen::VCard::Address::Type> types = QList<Jreen::VCard::Address::Type>()
			<< Jreen::VCard::Address::Home
			<< Jreen::VCard::Address::Work;

	DataItem addrItem(titles()->at(getAddressType(address)));
	if (d->accountInfo) {
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
	addItem(type, group, d_func()->accountInfo ? data : QVariant(data.split(',', QString::SkipEmptyParts)));
}

void JInfoRequest::addItemList(DataType type, DataItem &group, const QStringList &data) const
{
	addItem(type, group, data);
}

void JInfoRequest::onIqReceived(const Jreen::IQ &iq, int)
{
	Q_D(JInfoRequest);
	if(!iq.containsPayload<Jreen::VCard>())
		return;
	Account *acc = d->manager->account();
	QString id = iq.from().full();
	if (acc->id() != id)
		return;
	if (iq.subtype() == Jreen::IQ::Result) {
		setState(Updated);
		d->manager->handleVCard(d->vcard, id);
	} else if (iq.subtype() == Jreen::IQ::Error){
		setState(Error);
	}
}


}
