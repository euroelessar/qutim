/****************************************************************************
 *  roster.cpp
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QVariant>

#include <qutim/contact.h>

#include "mrimdebug.h"
#include "mrimcontact.h"
#include "mrimaccount.h"
#include "mrimstatus.h"

#include "roster.h"

struct RosterPrivate
{
    typedef QMultiHash<QString,MrimContact*> ContactsHash;
    QList<quint32> handledTypes;
    QMap<quint32,QString> groups;
    ContactsHash contacts;
    MrimAccount* account;
};

Roster::Roster(MrimAccount* acc) : p(new RosterPrivate)
{    
    p->account = acc;
}

Roster::~Roster()
{
    //cleanup
    QList<MrimContact*> contacts = p->contacts.values();
	qDeleteAll(contacts);
}

QList<quint32> Roster::handledTypes()
{
    if (p->handledTypes.isEmpty())
    {
        p->handledTypes << MRIM_CS_CONTACT_LIST2
                        << MRIM_CS_USER_STATUS; //TODO: add more types
    }
    return p->handledTypes;
}
    
bool Roster::handlePacket(MrimPacket& packet)
{
    bool handled = true;
    
    switch (packet.msgType())
    {
    case MRIM_CS_CONTACT_LIST2:
        parseList(packet);
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

bool Roster::parseList(MrimPacket& packet)
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

bool Roster::parseGroups(MrimPacket& packet, quint32 count, const QString& mask)
{    
    RosterParseMultiMap parsedGroup;
    
    for (quint32 i=0; i < count; i++)
    {
        parsedGroup = parseByMask(packet,mask);
        p->groups[i] = parsedGroup.getString(0,true);
        debug(VeryVerbose)<<"MrimGroup (id=" << i << ", flags=" << parsedGroup.getUint(0) << ", name=" << p->groups[i] << ")";
    }
    return true;
}

bool Roster::parseContacts(MrimPacket& packet, const QString& mask)
{
    UserAgent agent;
    RosterParseMultiMap parsedContact;
    quint32 paramIndex = 0;
    quint32 contactId = 20;
    QString statusUri, statusTitle, statusDesc, phones;
    MrimContact *contact = 0;

    while(!packet.atEnd())
    {
        parsedContact = parseByMask(packet,mask);

        if (parsedContact.isEmpty())
                break;// TODO: learn why its possible
        contact = new MrimContact(p->account);
        //generated params
        contact->setContactId(contactId++);
        //numeric params
        paramIndex = 0;
        paramIndex++; //TODO: contact->setFlags(parsedContact.getUint(paramIndex++));
        contact->setGroupId(parsedContact.getUint(paramIndex++));
        paramIndex++; //TODO: contact->setServerFlags(parsedContact.getUint(paramIndex++));
        paramIndex++; //ignore numeric status, will determine by URI
        paramIndex++; //TODO: contact->setFeatureFlags(parsedContact.getUint(paramIndex)); //last known numeric param
        //string params
        paramIndex = 0;
        contact->setEmail(parsedContact.getString(paramIndex++));
        contact->setName(parsedContact.getString(paramIndex++,true));
        phones = parsedContact.getString(paramIndex++);
        statusUri = parsedContact.getString(paramIndex++);        
        statusTitle = parsedContact.getString(paramIndex++,true);//title
        statusDesc = parsedContact.getString(paramIndex++,true);//desc
        contact->setStatus(MrimStatus::fromString(statusUri,statusTitle,statusDesc));

        if (agent.parse(parsedContact.getString(paramIndex++)))
        {
            contact->setUserAgent(agent);
        }

        debug(Verbose)<<"New contact read:"<<*contact;
        addToList(contact);
	}
	return true;
}

RosterParseMultiMap Roster::parseByMask(MrimPacket& packet, const QString& mask)
{
    RosterParseMultiMap map;
    map.setInsertInOrder(true);

    quint32 num = 0;
    LPString str;
    QVariant val(qMetaTypeId<LPString>());

    foreach (QChar ch, mask)
    {	
        if (ch == 'u')
        {
            packet.readTo(num);
            map.insert(ch,num);
        }
        else if (ch == 's')
        {
            packet.readTo(str);
            val.setValue(str);
            map.insertMulti(ch,val);
        }
    }

    return map;
}

bool Roster::handleStatusChanged(MrimPacket &packet)
{
    bool res = false;
    quint32 statusNum, comSupport;
    QString statusUri, statusTitle, statusDescr, email, userAgent;

    packet.readTo(statusNum);
    packet.readTo(&statusUri);
    packet.readTo(&statusTitle,true);
    packet.readTo(&statusDescr,true);
    packet.readTo(&email);
    packet.readTo(comSupport);
    packet.readTo(&userAgent);

    MrimContact *cnt = getContact(email);

    if (cnt) {
        cnt->setStatus(MrimStatus::fromString(statusUri,statusTitle,statusDescr));        
        UserAgent ag;
        ag.parse(userAgent);

        if (!ag.isEmpty()) {
            cnt->setUserAgent(ag);
        }
        //cnt->setFeatureFlags(comSupport);
        res = true;
    }
    return res;
}

QString Roster::groupName(quint32 groupId) const
{
    QString group;

	if (groupId != 0 && groupId < uint(p->groups.count()))
    {
        group = p->groups[groupId];
    }
    else if (groupId == MRIM_PHONE_GROUP_ID)
    {
        group = tr("Phone contacts");
    }
    return group;
}

void Roster::addToList(MrimContact *cnt)
{
    Q_ASSERT(cnt);
    p->contacts.insertMulti(cnt->email(),cnt);
}

MrimContact *Roster::getContact(const QString& id)
{
    return p->contacts.value(id,0);
}

void Roster::handleLoggedOut()
{
    Status st(Status::Offline);

    foreach (MrimContact *cnt, p->contacts) {        
        cnt->setStatus(st);
    }
}
