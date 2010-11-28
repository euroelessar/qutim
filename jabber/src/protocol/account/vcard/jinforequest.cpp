#include "jinforequest.h"
#include "jvcardmanager.h"
#include <gloox/vcard.h>
#include <QDate>
#include <qutim/debug.h>

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
		"about"
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
	InfoRequest::State state;
	const VCard *vcard;
	DataItem *item;
	QMap<QString, DataItem> items;
};

JInfoRequest::JInfoRequest(JVCardManager *manager, const QString &contact)
	: d_ptr(new JInfoRequestPrivate)
{
	Q_D(JInfoRequest);
	manager->fetchVCard(contact, this);
	d->state = Request;
	d->vcard = 0;
	d->item = 0;
}

JInfoRequest::~JInfoRequest()
{
	Q_D(JInfoRequest);
	if (d->vcard)
		delete d->vcard;
}

void JInfoRequest::setFetchedVCard(VCard *vcard)
{
	Q_D(JInfoRequest);
	DataItem item;
	// General page
	{
		DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
		// name
		addItemList(Nick, general, vcard->nickname());
		addItemList(FirstName, general, vcard->name().given);
		addItemList(MiddleName, general, vcard->name().middle);
		addItemList(LastName, general, vcard->name().family);
		// birthday
		{
			QString date(QString::fromStdString(vcard->bday()));
			QDate isoDate = QDate::fromString(date, "yyyy-MM-dd");
			if (isoDate.isValid())
				date = isoDate.toString();
			addItem(Birthday, general, date);
		}
		// homepage
		addItemList(Homepage, general, vcard->url());
		// telephone
		{
			if (!vcard->telephone().empty()) {
				foreach (VCard::Telephone phone, vcard->telephone())
				{
					DataType type;
					if (phone.home)
						type = HomePhone;
					else if (phone.work)
						type = WorkPhone;
					else if (phone.cell)
						type = MobilePhone;
					else
						type = Phone;
					addItem(type, general, QString::fromStdString(phone.number));
				}
			} else {
				addItem(Phone, general, QString());
			}
		}
		// email
		{
			if (!vcard->emailAddresses().empty()) {
				foreach (VCard::Email email, vcard->emailAddresses())
				{
					DataType type;
					if (email.home)
						type = PersonalEmail;
					else if (email.work)
						type = WorkEmail;
					else
						type = Email;
					addItem(Email, general, QString::fromStdString(email.userid));
				}
			} else {
				addItem(Email, general, QString());
			}
		}
		item.addSubitem(general);
	}
	// Addresses page
	{
		DataItem addresses(QT_TRANSLATE_NOOP("ContactInfo", "Addresses"));
		if (!vcard->addresses().empty()) {
			foreach (VCard::Address address, vcard->addresses())
			{
				DataType type;
				if (address.home)
					type = HomeAddress;
				else if (address.work)
					type = WorkAddress;
				else
					type = Address;
				DataItem addrItem(names()->at(type), titles()->at(type), QVariant());
				addItem(Country, addrItem, address.ctry);
				addItem(Region, addrItem, address.region);
				addItem(City, addrItem, address.locality);
				addItem(Postcode, addrItem, address.pcode);
				addItem(Street, addrItem, address.street);
				addItem(ExtendedAddress, addrItem, address.extadd);
				addItem(Postbox, addrItem, address.pobox);
				addresses.addSubitem(addrItem);
			}
		} else {
			addItem(Address, addresses, QString());
		}
		item.addSubitem(addresses);
	}
	// Work page
	{
		DataItem work(QT_TRANSLATE_NOOP("ContactInfo", "Work"));
		addItem(OrgName, work, vcard->org().name);
		addItem(OrgUnit, work, vcard->org().units);
		addItem(Title, work, vcard->title());
		addItem(Role, work, vcard->role());
		item.addSubitem(work);
	}
	// About page
	{
		DataItem about(QT_TRANSLATE_NOOP("ContactInfo", "About"));
		addMultilineItem(About, about, vcard->desc());
		item.addSubitem(about);
	}

	d->item = new DataItem(item);
	d->vcard = vcard;
	d->state = Done;
	emit stateChanged(d->state);
}

DataItem JInfoRequest::item(const QString &name) const
{
	Q_D(const JInfoRequest);
	if (!d->item)
		return DataItem();
	else if (!name.isEmpty())
		return d->items.value(name);
	else
		return *d->item;
}

InfoRequest::State JInfoRequest::state() const
{
	return d_func()->state;
}

void JInfoRequest::addItem(DataType type, DataItem &group, const QVariant &data)
{
	DataItem item(names()->at(type), titles()->at(type), data);
	d_func()->items.insert(item.name(), item);
	group.addSubitem(item);
}

void JInfoRequest::addMultilineItem(DataType type, DataItem &group, const std::string &data_helper)
{
	QString data = QString::fromStdString(data_helper);
	DataItem item(names()->at(type), titles()->at(type), data);
	d_func()->items.insert(item.name(), item);
	item.setProperty("multiline", true);
	item.setProperty("hideTitle", true);
	group.addSubitem(item);
}

void JInfoRequest::addItem(DataType type, DataItem &group, const std::string &data)
{
	addItem(type, group, QString::fromStdString(data));
}

void JInfoRequest::addItemList(DataType type, DataItem &group, const std::string &data)
{
	addItem(type, group, QString::fromStdString(data).split(',', QString::SkipEmptyParts));
}

void JInfoRequest::addItem(DataType type, DataItem &group, const StringList &data)
{
	QStringList result;
	foreach (const std::string &str, data)
		result << QString::fromStdString(str);
	addItem(type, group, result);
}
}
