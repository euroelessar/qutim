#include "jinforequest.h"
#include "jvcardmanager.h"
#include <gloox/vcard.h>

namespace Jabber
{
	struct JInfoRequestPrivate
	{
		QHash<QString, InfoItem> items;
		InfoRequest::State state;
	};

	JInfoRequest::JInfoRequest(JVCardManager *manager, const QString &contact)
			: d_ptr(new JInfoRequestPrivate)
	{
		manager->fetchVCard(contact, this);
		d_func()->items.clear();
		d_func()->state = Request;
	}

	JInfoRequest::~JInfoRequest()
	{

	}

	void JInfoRequest::setFetchedVCard(const VCard *vcard)
	{
		Q_D(JInfoRequest);
		d->items.insert("birthday", InfoItem(LocalizedString("Birthday"), QString::fromStdString(vcard->bday())));
		/*updatePhoto(avatarUrl, (avatarUrl != ""));
		if (utils::fromStd(m_vcard->formattedname()) != "")
			addName(utils::fromStd(m_vcard->formattedname()));
		if (utils::fromStd(m_vcard->nickname()) != "")
			addNick(utils::fromStd(m_vcard->nickname()));
		if (utils::fromStd(m_vcard->bday()) != "")
			addBirthday(utils::fromStd(m_vcard->bday()));
		if (utils::fromStd(m_vcard->url()) != "")
			addUrl(utils::fromStd(m_vcard->url()));
		if (!utils::fromStd(m_vcard->desc()).isEmpty())
			addDescription(utils::fromStd(m_vcard->desc()));
		if (m_vcard->emailAddresses().size())
			foreach (VCard::Email email, m_vcard->emailAddresses())
			{
				QString type;
				if (email.home)
					type = VCardConst::personalMailStatus();
				else if (email.work)
					type = VCardConst::workMailStatus();
				else
					type = VCardConst::emptyMailStatus();
				addEmail(utils::fromStd(email.userid), type);
			}
		if (m_vcard->telephone().size())
			foreach (VCard::Telephone phone, m_vcard->telephone())
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
				addPhone(utils::fromStd(phone.number), type);
			}
		if (m_vcard->addresses().size())
			foreach (VCard::Address address, m_vcard->addresses())
			{
				if (address.home)
				{
					if (utils::fromStd(address.ctry) != "")
						addHomeCountry(utils::fromStd(address.ctry));
					if (utils::fromStd(address.region) != "")
						addHomeRegion(utils::fromStd(address.region));
					if (utils::fromStd(address.locality) != "")
						addHomeCity(utils::fromStd(address.locality));
					if (utils::fromStd(address.pcode) != "")
						addHomePostcode(utils::fromStd(address.pcode));
					if (utils::fromStd(address.street) != "")
						addHomeStreet(utils::fromStd(address.street));
					if (utils::fromStd(address.pobox) != "")
						addHomePostbox(utils::fromStd(address.pobox));
				}
				if (address.work)
				{
					if (utils::fromStd(address.ctry) != "")
						addWorkCountry(utils::fromStd(address.ctry));
					if (utils::fromStd(address.region) != "")
						addWorkRegion(utils::fromStd(address.region));
					if (utils::fromStd(address.locality) != "")
						addWorkCity(utils::fromStd(address.locality));
					if (utils::fromStd(address.pcode) != "")
						addWorkPostcode(utils::fromStd(address.pcode));
					if (utils::fromStd(address.street) != "")
						addWorkStreet(utils::fromStd(address.street));
					if (utils::fromStd(address.pobox) != "")
						addWorkPostbox(utils::fromStd(address.pobox));
				}
			}
		if (utils::fromStd(m_vcard->org().name) != "")
			addOrgName(utils::fromStd(m_vcard->org().name));
		if (!m_vcard->org().units.empty() && utils::fromStd(m_vcard->org().units.front()) != "")
			addOrgUnit(utils::fromStd(m_vcard->org().units.front()));
		if (utils::fromStd(m_vcard->title()) != "")
			addTitle(utils::fromStd(m_vcard->title()));
		if (utils::fromStd(m_vcard->role()) != "")
			addRole(utils::fromStd(m_vcard->role()));*/
		delete vcard;
		d->state = Done;
		emit stateChanged(d->state);
	}

	QList<InfoItem> JInfoRequest::items() const
	{
		return d_func()->items.values();
	}

	InfoItem JInfoRequest::item(const QString &name) const
	{
		return d_func()->items.value(name);
	}

	InfoRequest::State JInfoRequest::state() const
	{
		return d_func()->state;
	}
}
