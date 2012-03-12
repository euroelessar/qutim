/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QVariant>

#include <qutim/contact.h>

#include "mrimdebug.h"
#include "mrimcontact.h"
#include "mrimaccount.h"
#include "mrimstatus.h"

#include "roster.h"
#include <qutim/rosterstorage.h>

class MrimRosterPrivate // : public RosterStorage
{
public:
    typedef QMultiHash<QString,MrimContact*> ContactsHash;
    QList<quint32> handledTypes;
    QMap<quint32,QString> groups;
    ContactsHash contacts;
    MrimAccount* account;
};

MrimRoster::MrimRoster(MrimAccount* acc) : p(new MrimRosterPrivate)
{    
    p->account = acc;
}

MrimRoster::~MrimRoster()
{
    //cleanup
    QList<MrimContact*> contacts = p->contacts.values();
	qDeleteAll(contacts);
}

QList<quint32> MrimRoster::handledTypes()
{
    if (p->handledTypes.isEmpty())
    {
        p->handledTypes
				<< MRIM_CS_CONTACT_LIST2
				<< MRIM_CS_USER_INFO
				<< MRIM_CS_MAILBOX_STATUS
				<< MRIM_CS_AUTHORIZE_ACK
				<< MRIM_CS_USER_STATUS; //TODO: add more types
    }
    return p->handledTypes;
}
    
bool MrimRoster::handlePacket(MrimPacket& packet)
{
    bool handled = true;
    
    switch (packet.msgType())
    {
    case MRIM_CS_CONTACT_LIST2:
        parseList(packet);
        break;
    case MRIM_CS_USER_INFO:
		handleUserInfo(packet);
		break;
    case MRIM_CS_USER_STATUS:
        handleStatusChanged(packet);
        break;
    default:
        handled = false;
        break;
    }
    return handled;
}

bool MrimRoster::parseList(MrimPacket& packet)
{
    quint32 opResult;
    packet.readTo(opResult);
    
    if (opResult == GET_CONTACTS_OK)
    {
        quint32 grCount = 0;    
        packet.readTo(grCount);
        
        QString grMask, contMask;
        packet.readTo(&grMask);
        packet.readTo(&contMask);
        
        if (parseGroups(packet,grCount,grMask))
        {
            parseContacts(packet,contMask);                    
        }
    }
    //TODO: handle errors
	return true;
}

bool MrimRoster::parseGroups(MrimPacket& packet, quint32 count, const QString& mask)
{
	for (quint32 i = 0; i < count; i++)
	{
		MrimRosterResult parsedGroup = parseByMask(packet, mask);
		quint32 flags = parsedGroup.getUInt(0);
		QString name = parsedGroup.getString(1, true);
		p->groups[i] = name;
		debug(VeryVerbose)<<"MrimGroup (id=" << i << ", flags=" << flags << ", name=" << name << ")";
	}
	return true;
}

enum MrimContactField
{
	MrimContactFlags = 0,
	MrimContactGroup,
	MrimContactEMail, // ansi
	MrimContactNick, // unicode
	MrimContactServerFlags,
	MrimContactStatus,
	MrimContactPhoneNumbers, // ansi
	MrimContactStatusUri, // ansi
	MrimContactStatusTitle, // unicode
	MrimContactStatusDescription, // unicode
	MrimContactComSupport,
	MrimContactUserAgent
};

bool MrimRoster::parseContacts(MrimPacket& packet, const QString& mask)
{
	MrimUserAgent agent;
	quint32 contactId = 20;
	QString statusUri, statusTitle, statusDesc, phones;
	MrimContact *contact = 0;
	
	QSet<QString> removedContacts;
	foreach (MrimContact *contact, p->contacts)
		removedContacts << contact->email();
	
	while(!packet.atEnd())
	{
		MrimRosterResult parsedContact = parseByMask(packet,mask);
	
		if (parsedContact.isEmpty())
			break;// TODO: learn why its possible
		QString id = parsedContact.getString(MrimContactEMail, false);
		removedContacts.remove(id);
		contact = p->contacts.value(id);
		bool newContact = !contact;
		if (newContact)
			contact = new MrimContact(id, p->account);
		contact->setContactInList(true);
		contact->setContactId(contactId++);
		contact->setFlags(static_cast<MrimContact::ContactFlag>(parsedContact.getUInt(MrimContactFlags)));
		contact->setServerFlags(parsedContact.getUInt(MrimContactServerFlags));
		contact->setFeatureFlags(static_cast<MrimConnection::FeatureFlag>(parsedContact.getUInt(MrimContactComSupport)));
		contact->setGroupId(parsedContact.getUInt(MrimContactGroup));
		contact->setContactName(parsedContact.getString(MrimContactNick, true));
		phones = parsedContact.getString(MrimContactPhoneNumbers, false);
		statusUri = parsedContact.getString(MrimContactStatusUri, false);
		statusTitle = parsedContact.getString(MrimContactStatusTitle, true);
		statusDesc = parsedContact.getString(MrimContactStatusDescription, true);
		MrimStatus status(statusUri, statusTitle, statusDesc);
		agent.parse(parsedContact.getString(MrimContactUserAgent, false));
		if (status != Status::Offline)
			status.setUserAgent(agent);
		status.setFlags(contact->serverFlags());
		contact->setStatus(status);
	
		contact->setUserAgent(agent);
	
		debug(Verbose)<<"New contact read:"<<*contact;
		if (newContact)
			addToList(contact);
	}
	foreach (const QString &id, removedContacts)
		p->contacts.value(id)->setContactInList(false);
	return true;
}

MrimRosterResult MrimRoster::parseByMask(MrimPacket& packet, const QString& mask)
{
	MrimRosterResult result;
	LPString str;
	quint32 val = 0;
	for (int i = 0; i < mask.size(); i++) {
		if (mask[i] == 's') {
			packet.readTo(str);
			result << qVariantFromValue(str);
		} else if (mask[i] == 'u') {
			packet.readTo(val);
			result << val;
		} else {
			debug() << mask[i];
			Q_ASSERT(!"Don't know what to do with this data");
			result << QVariant();
		}
	}
	return result;
}

bool MrimRoster::handleStatusChanged(MrimPacket &packet)
{
    quint32 statusNum, comSupport;
    QString statusUri, statusTitle, statusDescr, email, userAgent;

    packet.readTo(statusNum);
    packet.readTo(&statusUri);
    packet.readTo(&statusTitle,true);
    packet.readTo(&statusDescr,true);
    packet.readTo(&email);
    packet.readTo(comSupport);
    packet.readTo(&userAgent);

    MrimContact *contact = getContact(email, true);
	MrimStatus status(statusUri, statusTitle, statusDescr);
	MrimUserAgent ag;
	ag.parse(userAgent);
	status.setUserAgent(ag);
	status.setFlags(contact->serverFlags());
	contact->setStatus(status);
	contact->setFeatureFlags(static_cast<MrimConnection::FeatureFlag>(comSupport));
	contact->setUserAgent(ag);
	return true;
}

QString MrimRoster::groupName(quint32 groupId) const
{
    QString group;

	if (groupId < uint(p->groups.count()))
    {
        group = p->groups[groupId];
    }
    else if (groupId == MRIM_PHONE_GROUP_ID)
    {
        group = tr("Phone contacts");
    }
    return group;
}

void MrimRoster::addToList(MrimContact *cnt)
{
    Q_ASSERT(cnt);
    p->contacts.insertMulti(cnt->email(),cnt);
	emit p->account->contactCreated(cnt);
}

void MrimRoster::handleUserInfo(MrimPacket &packet)
{
	QMap<QString, QString> data;
	QString key, value;
	while (!packet.atEnd()) {
		packet.readTo(&key, false);
		packet.readTo(&value, true);
		data.insert(key, value);
	}
	p->account->setUserInfo(data);
}

void MrimRoster::handleAuthorizeAck(MrimPacket &packet)
{
	QString email;
	packet.readTo(&email);
	if (MrimContact *contact = p->contacts.value(email)) {
		quint32 serverFlags = contact->serverFlags();
		serverFlags &= ~CONTACT_INTFLAG_NOT_AUTHORIZED;
		contact->setServerFlags(serverFlags);
		MrimStatus status = contact->mrimStatus();
		status.setFlags(serverFlags);
		contact->setStatus(status);
	}
}

MrimContact *MrimRoster::getContact(const QString& id, bool create)
{
    MrimContact *contact = p->contacts.value(id);
	if (!contact && create) {
		contact = new MrimContact(id, p->account);
		contact->setContactInList(false);
		addToList(contact);
	}
	return contact;
}

void MrimRoster::handleLoggedOut()
{
    Status st(Status::Offline);

    foreach (MrimContact *cnt, p->contacts) {        
        cnt->setStatus(st);
    }
}

