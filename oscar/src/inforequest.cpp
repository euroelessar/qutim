#include "inforequest_p.h"
#include "icqcontact_p.h"
#include "icqaccount.h"

Q_DECLARE_METATYPE(QHostAddress);

namespace qutim_sdk_0_3 {

namespace oscar {

IcqInfoRequest::IcqInfoRequest(IcqAccount *account) :
	m_accountInfo(true), m_account(account)
{
	m_metaReq = new FullInfoMetaRequest(account);
	init();
}

IcqInfoRequest::IcqInfoRequest(IcqContact *contact) :
	m_accountInfo(false), m_contact(contact)
{
	m_metaReq = new FullInfoMetaRequest(contact->account(), contact);
	init();
}

IcqInfoRequest::~IcqInfoRequest()
{
	if (m_metaReq)
		m_metaReq->deleteLater();
}

InfoItem IcqInfoRequest::item(const QString &name) const
{
	if (name.isEmpty()) {
		InfoItem item;
		// General page
		{
			InfoItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
			{
				InfoItem name(QT_TRANSLATE_NOOP("ContactInfo", "Name"));
				addItem(Nick, name);
				addItem(FirstName, name);
				addItem(LastName, name);
				general.addSubitem(name);
			}
			{
				InfoItem accountInfo(QT_TRANSLATE_NOOP("ContactInfo", "Account info"));
				if (!m_accountInfo) {
					accountInfo.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "UIN"), m_contact->id()));
					accountInfo.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Registration"),
													m_contact->d_func()->regTime.toLocalTime()));
					QString emailStr = m_values.value(Email).toString();
					if (!emailStr.isEmpty() && !m_values.value(PublishPrimaryEmailFlag).toBool())
						emailStr += QT_TRANSLATE_NOOP("ContactInfo", " (private)");
					accountInfo.addSubitem(InfoItem("email", QT_TRANSLATE_NOOP("ContactInfo", "Email"), emailStr));
				} else {
					accountInfo.addSubitem(InfoItem("email", QT_TRANSLATE_NOOP("ContactInfo", "Email"),
									m_values.value(Email).toString()));
					// addItem(PublishPrimaryEmailFlag, accountInfo); TODO:
				}
				general.addSubitem(accountInfo);
			}
			item.addSubitem(general);
		}
		// Home page
		{
			InfoItem home(QT_TRANSLATE_NOOP("ContactInfo", "Home"));
			{
				InfoItem address(QT_TRANSLATE_NOOP("ContactInfo", "Home address"));
				addItem(HomeCountry, address);
				addItem(HomePhone, address);
				addItem(HomeCity, address);
				addItem(HomeFax, address);
				addItem(HomeState, address);
				addItem(HomeZipCode, address);
				addItem(CellPhone, address);
				addItem(HomeAddress, address);
				home.addSubitem(address);
			}
			{
				InfoItem originally(QT_TRANSLATE_NOOP("ContactInfo", "Originally from"));
				addItem(OriginalCountry, originally);
				addItem(OriginalCity, originally);
				addItem(OriginalState, originally);
				home.addSubitem(originally);
			}
			item.addSubitem(home);
		}
		// Work page
		{
			InfoItem work(QT_TRANSLATE_NOOP("ContactInfo", "Work"));
			{
				InfoItem address(QT_TRANSLATE_NOOP("ContactInfo", "Work address"));
				addItem(WorkCountry, address);
				addItem(WorkPhone, address);
				addItem(WorkCity, address);
				addItem(WorkFax, address);
				addItem(WorkState, address);
				addItem(WorkZip, address);
				addItem(WorkAddress, address);
				work.addSubitem(address);
			}
			{
				InfoItem company(QT_TRANSLATE_NOOP("ContactInfo", "Company"));
				addItem(WorkCompany, company);
				addItem(WorkOccupation, company);
				addItem(WorkDepartment, company);
				addItem(WorkPosition, company);
				addItem(WorkWebpage, company);
				work.addSubitem(company);
			}
			item.addSubitem(work);
		}
		// Personal page
		{
			InfoItem personal(QT_TRANSLATE_NOOP("ContactInfo", "Personal"));
			{
				InfoItem data(QT_TRANSLATE_NOOP("ContactInfo", "Personal"));
				addItem(Gender, data);
				//addItem(Homepage, data); TODO: fix saving
				addItem(Age, data);
				addItem(Languages, data);
				personal.addSubitem(data);
			}
			{
				addItem(Interests, personal);
			}
			item.addSubitem(personal);
		}
		// About
		{
			InfoItem about(QT_TRANSLATE_NOOP("ContactInfo", "About"));
			{
				InfoItem notes("notes", QT_TRANSLATE_NOOP("ContactInfo", "Notes"), m_values.value(Notes));
				notes.setProperty("hideTitle", true);
				notes.setProperty("multiline", true);
				about.addSubitem(notes);
			}
			item.addSubitem(about);
		}
		// Other
		if (!m_accountInfo){
			InfoItem otherGroup(QT_TRANSLATE_NOOP("ContactInfo", "Other"));
			otherGroup.setProperty("additional", true);
			DirectConnectionInfo info = m_contact->dcInfo();
			otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Internal IP"), QVariant::fromValue(info.internal_ip)));
			otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "External IP"), QVariant::fromValue(info.external_ip)));
			otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Port"), info.port));
			otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Protocol version"), info.protocol_version));
			{
				QStringList caps;
				foreach (const Capability &cap, m_contact->capabilities())
					caps << cap.name();
				otherGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Capabilities"), caps));
			}
			{
				InfoItem dcGroup(QT_TRANSLATE_NOOP("ContactInfo", "Direct connection extra info"));
				dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Last info update"),
											   QString("0x%1").arg(info.info_utime, 0, 16)));
				dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Last ext info update"),
											   QString("0x%1").arg(info.extinfo_utime, 0, 16)));
				dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Last ext status update"),
											   QString("0x%1").arg(info.extstatus_utime, 0, 16)));
				dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Client features"),
											   QString("0x%1").arg(info.client_features, 0, 16)));
				dcGroup.addSubitem(InfoItem(QT_TRANSLATE_NOOP("ContactInfo", "Cookie"), info.auth_cookie));
				otherGroup.addSubitem(dcGroup);
			}
			item.addSubitem(otherGroup);
		}
		return item;
	} else {
		MetaInfoField field(name);
		return InfoItem(name, field.toString(), m_values.value(field));
	}
}

void IcqInfoRequest::addItem(const MetaInfoField &field, InfoItem &group) const
{
	InfoItem item;
	item.setName(field.name());
	LocalizedStringList alt = field.titleAlternatives();
	if (!alt.isEmpty())
		item.setProperty("titleAlternatives", QVariant::fromValue(alt));
	alt = field.alternatives();
	if (!alt.isEmpty())
		item.setProperty("alternatives", QVariant::fromValue(alt));
	InfoItem def = item;
	QVariant data = m_values.value(field);
	if (data.isNull()) {
		data = field.defaultValue();
		item.setProperty("notSet", true);
	}
	item.setTitle(field.toString());
	if (data.type() == QVariant::StringList) {
		def.setData(QVariant(QVariant::String));
		def.setProperty("hideTitle", true);
		item.setMultiple(def, 3);
	}
	if (data.canConvert<CategoryList>()) {
		foreach (const Category &cat, data.value<CategoryList>()) {
			InfoItem subitem = def;
			subitem.setName(field.name());
			subitem.setTitle(cat.category);
			subitem.setData(cat.keyword);
			item.addSubitem(subitem);
		}
		def.setTitle(QString());
		item.setMultiple(def, 3);
	} else {
		item.setData(data);
	}
	group.addSubitem(item);
}

static void itemToMetaInfoValuesHashHelper(const InfoItem &items, MetaInfoValuesHash &hash)
{
	foreach (const InfoItem &item, items.subitems()) {
		if (item.isMultiple()) {
			CategoryList list;
			foreach (const InfoItem &catItem, item.subitems()) {
				Category cat;
				cat.category = catItem.title();
				cat.keyword = catItem.data().toString();
				list << cat;
			}
			hash.insert(item.name(), QVariant::fromValue(list));
		} else if (item.hasSubitems()) {
			itemToMetaInfoValuesHashHelper(item, hash);
		} else {
			hash.insert(item.name(), item.data());
		}
	}
}

MetaInfoValuesHash IcqInfoRequest::itemToMetaInfoValuesHash(const InfoItem &item)
{
	Q_ASSERT(!item.isNull());
	MetaInfoValuesHash hash;
	itemToMetaInfoValuesHashHelper(item, hash);
	return hash;
}

IcqInfoRequest::State IcqInfoRequest::state() const
{
	return m_state;
}

void IcqInfoRequest::init()
{
	m_state = Request;
	connect(m_metaReq, SIGNAL(done(bool)), SLOT(onDone(bool)));
	m_metaReq->send();
}

void IcqInfoRequest::onDone(bool ok)
{
	Q_ASSERT(sender());
	if (m_metaReq != sender())
		return;
	if (ok) {
		m_values = m_metaReq->values();
		m_state = Done;
	} else {
		m_state = Cancel;
	}
	emit stateChanged(m_state);
	m_metaReq->deleteLater();
}

} } // namespace qutim_sdk_0_3::oscar
