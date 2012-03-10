/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "inforequest_p.h"
#include "icqcontact_p.h"
#include "icqaccount.h"
#include "icqprotocol.h"
#include <qutim/notification.h>
#include <qutim/icon.h>

Q_DECLARE_METATYPE(QHostAddress)

namespace qutim_sdk_0_3 {

namespace oscar {

static bool isStatusOnline(const Status &status)
{
	Status::Type type = status.type();
	return type != Status::Offline && type != Status::Connecting;
}

IcqInfoRequestFactory::IcqInfoRequestFactory(IcqAccount *account) :
	QObject(account), m_account(account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

IcqInfoRequestFactory::SupportLevel IcqInfoRequestFactory::supportLevel(QObject *object)
{
	if (m_account == object) {
		return isStatusOnline(m_account->status()) ? ReadWrite : Unavailable;
	} else if (IcqContact *contact = qobject_cast<IcqContact*>(object)) {
		if (contact->id().contains("chat.facebook.com"))
			return NotSupported;
		IcqAccount *account = contact->account();
		if (account == m_account)
			return isStatusOnline(account->status()) ? ReadOnly : Unavailable;
	}
	return IcqInfoRequestFactory::NotSupported;
}

InfoRequest *IcqInfoRequestFactory::createrDataFormRequest(QObject *object)
{
	if (m_account == object) {
		return new IcqInfoRequest(m_account);
	} else if (IcqContact *contact = qobject_cast<IcqContact*>(object)) {
		if (contact->account() == m_account)
			return new IcqInfoRequest(contact);
	}
	return 0;
}

bool IcqInfoRequestFactory::startObserve(QObject *object)
{
	if (m_account == object) {
		return true;
	} else if (IcqContact *contact = qobject_cast<IcqContact*>(object)) {
		if (contact->account() == m_account) {
			m_contacts.insert(contact);
			return true;
		}
	}
	return false;
}

bool IcqInfoRequestFactory::stopObserve(QObject *object)
{
	if (m_account == object) {
		return true;
	} else if (IcqContact *contact = qobject_cast<IcqContact*>(object)) {
		return m_contacts.remove(contact) != 0;
	}
	return false;
}

void IcqInfoRequestFactory::onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
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

	SupportLevel level = supported ? ReadWrite : Unavailable;
	setSupportLevel(m_account, level);

	level = supported ? ReadOnly : Unavailable;
	foreach (Contact *contact, m_contacts)
		setSupportLevel(contact, level);

}

IcqInfoRequest::IcqInfoRequest(IcqAccount *account) :
	InfoRequest(account), m_accountInfo(true), m_account(account)
{
}

IcqInfoRequest::IcqInfoRequest(IcqContact *contact) :
	InfoRequest(contact), m_accountInfo(false), m_contact(contact)
{
}

IcqInfoRequest::~IcqInfoRequest()
{
	if (m_request)
		m_request.data()->deleteLater();
	if (m_updater)
		m_updater.data()->deleteLater();
}

DataItem IcqInfoRequest::createDataItem() const
{
	DataItem item;
	// General page
	{
		DataItem general(QT_TRANSLATE_NOOP("ContactInfo", "General"));
		{
			DataItem avatarItem(QLatin1String("avatar"),
								QT_TRANSLATE_NOOP("ContactInfo", "Avatar"),
								QPixmap(m_object->property("avatar").toString()));
			avatarItem.setProperty("hideTitle", true);
			avatarItem.setProperty("imageSize", QSize(64, 64));
			avatarItem.setProperty("defaultImage", Icon(QLatin1String("qutim")).pixmap(64));
			general.addSubitem(avatarItem);
		}
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
}

QVariant IcqInfoRequest::getValue(const QString &name) const
{
	return m_values.value(name);
}

void IcqInfoRequest::addItem(const MetaField &field, DataItem &group) const
{
	group.addSubitem(field.toDataItem(m_values.value(field)));
}

void IcqInfoRequest::doRequest(const QSet<QString> &hints)
{
	static QSet<QString> m_shortInfo = QSet<QString>()
			<< "nick" << "firstName" << "lastName" << "email" << "gender";
	bool requestFullInfo = hints.isEmpty();
	if (!requestFullInfo)
		requestFullInfo = !((hints - m_shortInfo).isEmpty());

	if (requestFullInfo) {
		m_request = m_accountInfo ?
					new FullInfoMetaRequest(m_account) :
					new FullInfoMetaRequest(m_contact);
	} else {
		m_request = m_accountInfo ?
					new ShortInfoMetaRequest(m_account) :
					new ShortInfoMetaRequest(m_contact);
	}
	connect(m_request.data(), SIGNAL(done(bool)), SLOT(onRequestDone(bool)));
	m_request.data()->send();
	setState(InfoRequest::Requesting);
}

void IcqInfoRequest::doUpdate(const DataItem &dataItem)
{
	if (!m_accountInfo) {
		setErrorString("Cannot update information about a contact");
		setState(InfoRequest::Error);
		return;
	}

	setState(InfoRequest::Updating);
	m_account->setAvatar(dataItem.subitem("avatar").property("imagePath", QString()));
	MetaInfoValuesHash values = MetaField::dataItemToHash(dataItem, true);
	m_account->setName(values.value(Nick, m_account->id()).toString());
	m_updater = new UpdateAccountInfoMetaRequest(m_account, values);
	connect(m_updater.data(), SIGNAL(done(bool)), SLOT(onInfoUpdated()));
	m_updater.data()->send();
}

void IcqInfoRequest::doCancel()
{
	if (m_request)
		m_request.data()->deleteLater();
	if (m_updater)
		m_updater.data()->deleteLater();
	setState(InfoRequest::Canceled);
}

void IcqInfoRequest::onRequestDone(bool ok)
{
	if (ok) {
		m_values = m_request.data()->values();
		setState(InfoRequest::RequestDone);
		if (m_accountInfo)
			m_account->setName(m_request.data()->value<QString>(Nick, m_account->id()));
	} else {
		handleError(m_request.data());
	}
	m_request.data()->deleteLater();
}

void IcqInfoRequest::onInfoUpdated(bool ok)
{
	if (ok)
		setState(InfoRequest::Updated);
	else
		handleError(m_updater.data());
	m_updater.data()->deleteLater();
}

void IcqInfoRequest::handleError(AbstractMetaRequest *request)
{
	AbstractMetaRequest::ErrorType error = request->errorType();
	if (error == AbstractMetaRequest::ProtocolError ||
		error == AbstractMetaRequest::Timeout)
	{
		// Do we really want to notify user about all this shit?
		/*
		NotificationRequest notifRequest(Notification::System);
		notifRequest.setObject(m_account);
		notifRequest.setText(request->errorString());
		notifRequest.send();
		*/

		setState(InfoRequest::Error);
		setErrorString(request->errorString());
	} else {
		setState(InfoRequest::Canceled);
	}
}


} } // namespace qutim_sdk_0_3::oscar

