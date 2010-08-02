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
				<< QT_TRANSLATE_NOOP("ContactInfo", "Middle name")
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
	
//	void init_genders(QList<LocalizedString> &genders)
//	{
//		genders << QT_TRANSLATE_NOOP("ContactInfo", "Undefined")
//				<< QT_TRANSLATE_NOOP("ContactInfo", "Female")
//				<< QT_TRANSLATE_NOOP("ContactInfo", "Male");
//	}
//	
//	Q_GLOBAL_STATIC_WITH_INITIALIZER(QList<LocalizedString>, genders, init_genders(*x))
	
	struct JInfoRequestPrivate
	{
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

	void JInfoRequest::setFetchedVCard(const VCard *vcard)
	{
		Q_D(JInfoRequest);
		DataItem item;
		// General page
		{
			DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
			{
				DataItem name(QT_TRANSLATE_NOOP("ContactInfo", "Name"));
				addItemList(Nick, name, vcard->nickname());
				addItemList(FirstName, name, vcard->name().given);
				addItemList(MiddleName, name, vcard->name().middle);
				addItemList(LastName, name, vcard->name().family);
				general.addSubitem(name);
			}
			item.addSubitem(general);
		}
		
		//updatePhoto(avatarUrl, (avatarUrl != ""));
		/*InfoItem general = InfoItem(LocalizedString("General"));
		if (!QString::fromStdString(vcard->formattedname()).isEmpty()) {
			InfoItem item(InfoItem("Name", QString::fromStdString(vcard->formattedname())));
			general.addSubitem(item);
			d->items.insert("Name", &item);
		}
		if (!QString::fromStdString(vcard->nickname()).isEmpty())
			general.addSubitem(InfoItem("Nick", QString::fromStdString(vcard->nickname())));
		if (!QString::fromStdString(vcard->bday()).isEmpty()) {
			QString date(QString::fromStdString(vcard->bday()));
			QDate isoDate = QDate::fromString(date, "yyyy-MM-dd");
			if (isoDate.isValid())
				date = isoDate.toString();
			general.addSubitem(InfoItem("Birthday", date));
		}
		if (!QString::fromStdString(vcard->url()).isEmpty())
			general.addSubitem(InfoItem("Homepage", QString::fromStdString(vcard->url())));
		if (!QString::fromStdString(vcard->desc()).isEmpty())
			general.addSubitem(InfoItem("About", QString::fromStdString(vcard->desc())));
		if (general.hasSubitems())
			d->vcard.addSubitem(general);
		if (vcard->emailAddresses().size())
			foreach (VCard::Email email, vcard->emailAddresses())
			{
				QString type;
				if (email.home)
					type = VCardConst::personalMailStatus();
				else if (email.work)
					type = VCardConst::workMailStatus();
				else
					type = VCardConst::emptyMailStatus();
				addEmail(QString::fromStdString(email.userid), type);
			}
		if (vcard->telephone().size())
			foreach (VCard::Telephone phone, vcard->telephone())
			{
				QString type;
				if (phone.home)
					type = VCardConst::homePhoneStatus();
				else if (phone.work)
					type = VCardConst::workPhoneStatus();
				else if (phone.cell)
					type = VCardConst::celluarPhoneStatus();
				else
					type = VCardConst::emptyPhoneStatus();
				addPhone(QString::fromStdString(phone.number), type);
			}
		if (vcard->addresses().size())
			foreach (VCard::Address address, vcard->addresses())
			{
				if (address.home)
				{
					if (QString::fromStdString(address.ctry) != "")
						addHomeCountry(QString::fromStdString(address.ctry));
					if (QString::fromStdString(address.region) != "")
						addHomeRegion(QString::fromStdString(address.region));
					if (QString::fromStdString(address.locality) != "")
						addHomeCity(QString::fromStdString(address.locality));
					if (QString::fromStdString(address.pcode) != "")
						addHomePostcode(QString::fromStdString(address.pcode));
					if (QString::fromStdString(address.street) != "")
						addHomeStreet(QString::fromStdString(address.street));
					if (QString::fromStdString(address.pobox) != "")
						addHomePostbox(QString::fromStdString(address.pobox));
				}
				if (address.work)
				{
					if (QString::fromStdString(address.ctry) != "")
						addWorkCountry(QString::fromStdString(address.ctry));
					if (QString::fromStdString(address.region) != "")
						addWorkRegion(QString::fromStdString(address.region));
					if (QString::fromStdString(address.locality) != "")
						addWorkCity(QString::fromStdString(address.locality));
					if (QString::fromStdString(address.pcode) != "")
						addWorkPostcode(QString::fromStdString(address.pcode));
					if (QString::fromStdString(address.street) != "")
						addWorkStreet(QString::fromStdString(address.street));
					if (QString::fromStdString(address.pobox) != "")
						addWorkPostbox(QString::fromStdString(address.pobox));
				}
			}
		if (QString::fromStdString(vcard->org().name) != "")
			addOrgName(QString::fromStdString(vcard->org().name));
		if (!vcard->org().units.empty() && QString::fromStdString(vcard->org().units.front()) != "")
			addOrgUnit(QString::fromStdString(vcard->org().units.front()));
		if (QString::fromStdString(vcard->title()) != "")
			addTitle(QString::fromStdString(vcard->title()));
		if (QString::fromStdString(vcard->role()) != "")
			addRole(QString::fromStdString(vcard->role()));*/

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

	void JInfoRequest::addItem(DataType type, DataItem &group, const std::string &data)
	{
		addItem(type, group, QString::fromStdString(data));
	}
	
	void JInfoRequest::addItemList(DataType type, DataItem &group, const std::string &data)
	{
		addItem(type, group, QString::fromStdString(data).split(','));
	}

	void JInfoRequest::addItem(DataType type, DataItem &group, const StringList &data)
	{
		QStringList result;
		foreach (const std::string &str, data)
			result << QString::fromStdString(str);
		addItem(type, group, result);
	}
}
