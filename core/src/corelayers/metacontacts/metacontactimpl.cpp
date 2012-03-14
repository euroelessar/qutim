/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "metacontactimpl.h"
#include "manager.h"
#include <qutim/debug.h>
#include <qutim/tooltip.h>
#include <QApplication>
#include <qutim/protocol.h>
#include <QLatin1Literal>
#include <qutim/rosterstorage.h>

using namespace qutim_sdk_0_3;

namespace Core
{
namespace MetaContacts
{
bool contactLessThan(Contact *a, Contact *b)
{
	static int priority [] = {
		1, // Online
		0, // FreeChat
		2, // Away
		3, // NA
		3, // DND
		2, // Invisible
		4  // Offline
	};
	return priority[a->status().type()] < priority[b->status().type()];
}

MetaContactImpl::MetaContactImpl(const QString &id) : m_id(id)
{

}

MetaContactImpl::~MetaContactImpl()
{
	//			static_cast<Manager*>(account())->removeContact(m_id);
}

QString MetaContactImpl::id() const
{
	return m_id;
}

QString MetaContactImpl::avatar() const
{
	return m_lastAvatar;
}

void MetaContactImpl::setName(const QString &name)
{
	setContactName(name);
	RosterStorage::instance()->updateContact(this);
}

Status MetaContactImpl::status() const
{
	return m_status;
}

void MetaContactImpl::setTags(const QStringList &tags)
{
	setContactTags(tags);
	RosterStorage::instance()->updateContact(this);
}

bool MetaContactImpl::sendMessage(const Message &message)
{
	//TODO implement logic
    debug() << ">>>ACTIVE" << m_active_contact->account()->name();
    if (m_active_contact->sendMessage(message))
    {
        return true;
    }
    else
    {
        return false;
    }

    /*for (int i = 0; i < m_contacts.size(); i++) {
        if (m_contacts.at(i)->sendMessage(message))
			return true;
	}
    return false;*/
}

void MetaContactImpl::addContact(Contact* contact, bool update)
{
	if (m_contacts.contains(contact) || (contact == this))
		return;

	if(update) {
		QStringList previous = m_tags;
		QStringList contactTags = contact->tags();
		for (int i = 0; i < contactTags.size(); i++) {
			if (!m_tags.contains(contactTags.at(i))) {
				m_tags << contactTags.at(i);
			}
		}
		emit tagsChanged(m_tags, previous);
	}
    //int index = qUpperBound(m_contacts.begin(), m_contacts.end(), contact, contactLessThan)
//		- m_contacts.begin();
    m_contacts.append(contact);
	MetaContact::addContact(contact);
    connect(contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onContactStatusChanged()));
	connect(contact, SIGNAL(avatarChanged(QString)),
			SLOT(setAvatar(QString)));


//	if (index == 0)
    //	resetStatus();
	if (m_contacts.size() == 1 || m_name.isEmpty())
		resetName();

	//setMenuOwner(contact); TODO, implement logic!
		
	if(update)
		RosterStorage::instance()->updateContact(this);
    setActiveContact();
}

void MetaContactImpl::addContact(Contact *contact)
{
	addContact(contact, true);
}

void MetaContactImpl::removeContact(Contact *contact)
{
	int index = m_contacts.indexOf(contact);
	if (index == -1)
		return;
	m_contacts.removeAt(index);
	MetaContact::removeContact(contact);
	if (m_contacts.count()) {
		resetStatus();
	} else {
		RosterStorage::instance()->removeContact(this);
		deleteLater();
	}
}

void MetaContactImpl::resetName()
{
	QString currentName;
	for (int i = 0; i < m_contacts.size(); i++) {
		currentName = m_contacts.at(i)->name();
		if (!currentName.isEmpty())
			break;
	}
	if (currentName != m_name) {
		QString previous = m_name;
		m_name = currentName;
		emit nameChanged(m_name, previous);
	}
}

void MetaContactImpl::resetStatus()
{
	if (m_contacts.isEmpty()) {
		if (m_status.type() == Status::Offline)
			return;
		Status previous = m_status;
		m_status = Status();
		emit statusChanged(m_status, previous);
		return;
	}
	Status previous = m_status;
	Status contactStatus = m_contacts.first()->status();
	if (contactStatus.type() == m_status.type()
			&& contactStatus.text() == m_status.text()) {
		return;
	}
	m_status = Status(contactStatus.type());
	m_status.setName(contactStatus.name());
	m_status.setText(contactStatus.text());
	QSet<QString> keys;
	for (int i = 0; i < m_contacts.size(); i++) {
		QHash<QString, QVariantHash> hash = m_contacts.at(i)->status().extendedInfos();
		QHash<QString, QVariantHash>::const_iterator it = hash.constBegin();
		QHash<QString, QVariantHash>::const_iterator endit = hash.constEnd();
		for (; it != endit; it++) {
			if (!keys.contains(it.key())) {
				keys << it.key();
				m_status.setExtendedInfo(it.key(), it.value());
			}
		}
	}
	emit statusChanged(m_status, previous);
}

void MetaContactImpl::onContactStatusChanged()
{
/*	Contact *contact = qobject_cast<Contact*>(sender());
	int oldIndex = m_contacts.indexOf(contact);
	int index = qUpperBound(m_contacts.begin(), m_contacts.end(), contact, contactLessThan)
			- m_contacts.begin();
	if (index != oldIndex && index != m_contacts.count())
		m_contacts.move(oldIndex, index);
	if (index == 0 || oldIndex == 0)
        resetStatus();*/
}

void MetaContactImpl::setAvatar(const QString& path)
{
	setContactAvatar(path);
	RosterStorage::instance()->updateContact(this);
}

qutim_sdk_0_3::ChatUnitList MetaContactImpl::lowerUnits()
{
	ChatUnitList list;
	for (int i = 0;i!=m_contacts.count();i++)
		list.append(m_contacts.at(i));
	return list;
}

const qutim_sdk_0_3::ChatUnit* MetaContactImpl::getHistoryUnit() const
{
	//implement logic
	return m_contacts.first();
}

bool MetaContactImpl::event(QEvent* ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		if (ev->type() == ToolTipEvent::eventType()) {
			ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
			if (event->generateLayout())
				Contact::event(ev);
			foreach (ChatUnit *contact, m_contacts) {
				ToolTipEvent contactEvent(false);
				qApp->sendEvent(contact, &contactEvent);
				QString text = contactEvent.html();
				if (!text.isEmpty())
					event->addHtml(QLatin1Literal("<br/><br/>") % text);
			}
			return true;
		}
	} else if(ev->type() == ChatStateEvent::eventType()) {
		ChatStateEvent *event = static_cast<ChatStateEvent*>(ev);
		//TODO implement logic
		qApp->sendEvent(m_contacts.first(),event);
	}
	return qutim_sdk_0_3::MetaContact::event(ev);
}

void MetaContactImpl::addContacts(QList<Contact*> contacts)
{
    connect(ChatLayer::instance(),SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)), this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));

    foreach(Contact *contact, contacts)
    {
        addContact(contact,false);
    }
}

void MetaContactImpl::setContactAvatar(const QString& path)
{
	m_lastAvatar = path;
	emit avatarChanged(m_lastAvatar);
}

void MetaContactImpl::setContactName(const QString& name)
{
	if (m_name == name)
		return;
	QString previous = m_name;
	m_name = name;
	emit nameChanged(m_name, previous);
}

void MetaContactImpl::setContactTags(const QStringList& tags)
{
	if(m_tags == tags)
		return;
	QStringList previous = m_tags;
	m_tags = tags;
	emit tagsChanged(m_tags,previous);
}

void MetaContactImpl::setActiveContact(Contact* contact)
{
    if(contact)
    {
        m_active_contact = contact;
        debug() << ">Active contact" << m_active_contact->buddy() << m_active_contact->name();
        return;
    }
    for (int i = 0; i < m_contacts.size(); i++)
    {
        if (m_contacts.at(i)->status().type() != Status::Offline)
        {
            m_active_contact = m_contacts.at(i);
            debug() << ">>Active contact" << m_active_contact->buddy() << m_active_contact->name() << m_active_contact;
            return;
        }
    }
    m_active_contact = m_contacts.at(0);
    debug() << ">>>Active contact" << m_active_contact->buddy() << m_active_contact->name();
}

void MetaContactImpl::onMessageReceived(Message *message)
{
    debug() << message->chatUnit();
    debug() << message->chatUnit()->account() << m_active_contact->account();
    if (!(m_active_contact->account() == message->chatUnit()->account()))
    {
        for (int i = 0; i < m_contacts.size(); i++)
        {
            if (m_contacts.at(i)->account() == message->chatUnit()->account())
            {
                m_active_contact = m_contacts.at(i);
                debug() << "New active contact: " << m_active_contact->account()->name() << m_active_contact->name();
                return;
            }
        }
        debug() << "NO ACTIVE CONTACT!!! WTF???";
    }
}

void MetaContactImpl::onSessionCreated(ChatSession *session)
{
    MetaContact *contact = qobject_cast<MetaContact*>(session->unit());
    if(contact)
    {
        if(contact == this->metaContact())
        {
            setActiveContact();
            debug() << "Session created" << contact->name();
      //      connect(session,SIGNAL(messageReceived(qutim_sdk_0_3::Message*)),this,SLOT(onMessageReceived(qutim_sdk_0_3::Message*)));
        }
        else
        {
            //debug() << this << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
            //debug() << contact->metaContact() << "!!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
        }
    }
    else
    {
            //debug()  << "Contact not found!";
    }

}
}
}

