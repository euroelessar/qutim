#include "jinforequest.h"
#include "jvcardmanager.h"
#include <gloox/vcard.h>
#include <QDate>
#include <qutim/debug.h>

namespace Jabber
{
	struct JInfoRequestPrivate
	{
		InfoRequest::State state;
		const VCard *vcard;
	};

	JInfoRequest::JInfoRequest(JVCardManager *manager, const QString &contact)
			: d_ptr(new JInfoRequestPrivate)
	{
		manager->fetchVCard(contact, this);
		d_func()->state = Request;
	}

	JInfoRequest::~JInfoRequest()
	{

	}

	void JInfoRequest::setFetchedVCard(const VCard *vcard)
	{
		Q_D(JInfoRequest);
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
		d->vcard = vcard;
		d->state = Done;
		emit stateChanged(d->state);
	}

	InfoItem JInfoRequest::item(const QString &name) const
	{
		return InfoItem();
	}

	InfoRequest::State JInfoRequest::state() const
	{
		return d_func()->state;
	}
}
