#include "inforequest_p.h"
#include "icqcontact_p.h"
#include "icqaccount.h"
#include <qutim/notification.h>

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
	m_metaReq = new FullInfoMetaRequest(contact);
	init();
}

IcqInfoRequest::~IcqInfoRequest()
{
	if (m_metaReq)
		m_metaReq->deleteLater();
}

DataItem IcqInfoRequest::item(const QString &name) const
{
	if (name.isEmpty()) {
		DataItem item;
		// General page
		{
			DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
			{
				DataItem name(QT_TRANSLATE_NOOP("ContactInfo", "Name"));
				addItem(Nick, name);
				addItem(FirstName, name);
				addItem(LastName, name);
				general.addSubitem(name);
			}
			{
				DataItem accountInfo(QT_TRANSLATE_NOOP("ContactInfo", "Account info"));
				if (!m_accountInfo) {
					accountInfo.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "UIN"), m_contact->id()));
					accountInfo.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Registration"),
													m_contact->d_func()->regTime.toLocalTime()));
					QString emailStr = m_values.value(Email).toString();
					if (!emailStr.isEmpty() && !m_values.value(PublishPrimaryEmailFlag).toBool())
						emailStr += QT_TRANSLATE_NOOP("ContactInfo", " (private)");
					accountInfo.addSubitem(DataItem("email", QT_TRANSLATE_NOOP("ContactInfo", "Email"), emailStr));
				} else {
					accountInfo.addSubitem(DataItem("email", QT_TRANSLATE_NOOP("ContactInfo", "Email"),
									m_values.value(Email).toString()));
					// addItem(PublishPrimaryEmailFlag, accountInfo); TODO:
				}
				general.addSubitem(accountInfo);
			}
			item.addSubitem(general);
		}
		// Home page
		{
			DataItem home(QT_TRANSLATE_NOOP("ContactInfo", "Home"));
			{
				DataItem address(QT_TRANSLATE_NOOP("ContactInfo", "Home address"));
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
				DataItem originally(QT_TRANSLATE_NOOP("ContactInfo", "Originally from"));
				addItem(OriginalCountry, originally);
				addItem(OriginalCity, originally);
				addItem(OriginalState, originally);
				home.addSubitem(originally);
			}
			item.addSubitem(home);
		}
		// Work page
		{
			DataItem work(QT_TRANSLATE_NOOP("ContactInfo", "Work"));
			{
				DataItem address(QT_TRANSLATE_NOOP("ContactInfo", "Work address"));
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
				DataItem company(QT_TRANSLATE_NOOP("ContactInfo", "Company"));
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
			DataItem personal(QT_TRANSLATE_NOOP("ContactInfo", "Personal"));
			{
				DataItem data(QT_TRANSLATE_NOOP("ContactInfo", "Personal"));
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
			DataItem about(QT_TRANSLATE_NOOP("ContactInfo", "About"));
			{
				DataItem notes("notes", QT_TRANSLATE_NOOP("ContactInfo", "Notes"), m_values.value(Notes));
				notes.setProperty("hideTitle", true);
				notes.setProperty("multiline", true);
				about.addSubitem(notes);
			}
			item.addSubitem(about);
		}
		// Other
		if (!m_accountInfo){
			DataItem otherGroup(QT_TRANSLATE_NOOP("ContactInfo", "Other"));
			otherGroup.setProperty("additional", true);
			DirectConnectionInfo info = m_contact->dcInfo();
			otherGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Internal IP"), QVariant::fromValue(info.internal_ip)));
			otherGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "External IP"), QVariant::fromValue(info.external_ip)));
			otherGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Port"), info.port));
			otherGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Protocol version"), info.protocol_version));
			{
				QStringList caps;
				foreach (const Capability &cap, m_contact->capabilities())
					caps << cap.name();
				otherGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Capabilities"), caps));
			}
			{
				DataItem dcGroup(QT_TRANSLATE_NOOP("ContactInfo", "Direct connection extra info"));
				dcGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Last info update"),
											   QString("0x%1").arg(info.info_utime, 0, 16)));
				dcGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Last ext info update"),
											   QString("0x%1").arg(info.extinfo_utime, 0, 16)));
				dcGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Last ext status update"),
											   QString("0x%1").arg(info.extstatus_utime, 0, 16)));
				dcGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Client features"),
											   QString("0x%1").arg(info.client_features, 0, 16)));
				dcGroup.addSubitem(DataItem(QT_TRANSLATE_NOOP("ContactInfo", "Cookie"), info.auth_cookie));
				otherGroup.addSubitem(dcGroup);
			}
			item.addSubitem(otherGroup);
		}
		return item;
	} else {
		MetaField field(name);
		return DataItem(name, field.toString(), m_values.value(field));
	}
}

void IcqInfoRequest::addItem(const MetaField &field, DataItem &group) const
{
	group.addSubitem(field.toDataItem(m_values.value(field)));
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
		if (m_accountInfo)
			m_account->setName(m_metaReq->value<QString>(Nick, m_account->id()));
	} else {
		m_state = Cancel;
		AbstractMetaRequest::ErrorType error = m_metaReq->errorType();
		if (error == AbstractMetaRequest::ProtocolError ||
			error == AbstractMetaRequest::Timeout)
		{
			NotificationRequest request(Notification::System);
			request.setObject(m_account);
			request.setText(m_metaReq->errorString());
			request.send();
		}
	}
	emit stateChanged(m_state);
	m_metaReq->deleteLater();
}

} } // namespace qutim_sdk_0_3::oscar
