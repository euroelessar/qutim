#include "jinforequest.h"
#include "jvcardmanager.h"
#include <QDate>
#include <qutim/debug.h>
#include <jreen/vcard.h>
#include <QUrl>
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
};

JInfoRequest::JInfoRequest(JVCardManager *manager, QObject *object)
	: InfoRequest(object), d_ptr(new JInfoRequestPrivate)
{
	Q_D(JInfoRequest);
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

Jreen::VCard::Ptr JInfoRequest::convert(const DataItem &item)
{
	Q_UNUSED(item);
	Jreen::VCard::Ptr vcard = Jreen::VCard::Ptr::create();
	return vcard;
}

DataItem JInfoRequest::createDataItem() const
{
	Q_D(const JInfoRequest);
	DataItem item;
	if (!d->vcard)
		return item;
	{
		DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
		// General page
		{
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
		{
			if (!d->vcard->telephones().empty()) {
				foreach (Jreen::VCard::Telephone phone, d->vcard->telephones())
				{
					DataType type;
					if (phone.testType(Jreen::VCard::Telephone::Home))
						type = HomePhone;
					else if (phone.testType(Jreen::VCard::Telephone::Work))
						type = WorkPhone;
					else if (phone.testType(Jreen::VCard::Telephone::Cell))
						type = MobilePhone;
					else
						type = Phone;
					addItem(type, general, phone.number());
				}
			} else {
				addItem(Phone, general, QString());
			}
		}
		//// email
		{
			if (!d->vcard->emails().empty()) {
				foreach (Jreen::VCard::EMail email, d->vcard->emails())
				{
					DataType type;
					if (email.testType(Jreen::VCard::EMail::Home))
						type = PersonalEmail;
					else if (email.testType(Jreen::VCard::EMail::Work))
						type = WorkEmail;
					else
						type = Email;
					addItem(Email, general,email.userId());
				}
			} else {
				addItem(Email, general, QString());
			}
		}
		item.addSubitem(general);
	}
	// Addresses page
	//{
	//	DataItem addresses(QT_TRANSLATE_NOOP("ContactInfo", "Addresses"));
	//	if (!d->vcard->addresses().empty()) {
	//		foreach (VCard::Address address, d->vcard->addresses())
	//		{
	//			DataType type;
	//			if (address.home)
	//				type = HomeAddress;
	//			else if (address.work)
	//				type = WorkAddress;
	//			else
	//				type = Address;
	//			DataItem addrItem(names()->at(type), titles()->at(type), QVariant());
	//			addItem(Country, addrItem, address.ctry);
	//			addItem(Region, addrItem, address.region);
	//			addItem(City, addrItem, address.locality);
	//			addItem(Postcode, addrItem, address.pcode);
	//			addItem(Street, addrItem, address.street);
	//			addItem(ExtendedAddress, addrItem, address.extadd);
	//			addItem(Postbox, addrItem, address.pobox);
	//			addresses.addSubitem(addrItem);
	//		}
	//	} else {
	//		addItem(Address, addresses, QString());
	//	}
	//	item.addSubitem(addresses);
	//}
	// Work page
	//{
	//	DataItem work(QT_TRANSLATE_NOOP("ContactInfo", "Work"));
	//	addItem(OrgName, work, d->vcard->org().name);
	//	addItem(OrgUnit, work, d->vcard->org().units);
	//	addItem(Title, work, d->vcard->title());
	//	addItem(Role, work, d->vcard->role());
	//	item.addSubitem(work);
	//}
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
	Jreen::VCard::Ptr vcard = convert(dataItem);
	d->manager->storeVCard(vcard);
	setState(InfoRequest::Canceled); // Operation is not supported yet
}

void JInfoRequest::doCancel()
{
	setState(InfoRequest::Canceled);
}

void JInfoRequest::addItem(DataType type, DataItem &group, const QVariant &data)
{
	DataItem item(names()->at(type), titles()->at(type), data);
	group.addSubitem(item);
}

void JInfoRequest::addMultilineItem(DataType type, DataItem &group, const QString &data)
{
	DataItem item(names()->at(type), titles()->at(type), data);
	item.setProperty("multiline", true);
	item.setProperty("hideTitle", true);
	group.addSubitem(item);
}

void JInfoRequest::addItemList(DataType type, DataItem &group, const QString &data)
{
	addItem(type, group,data.split(',', QString::SkipEmptyParts));
}

void JInfoRequest::addItemList(DataType type, DataItem &group, const QStringList &data)
{
	addItem(type,group,data);
}

}
