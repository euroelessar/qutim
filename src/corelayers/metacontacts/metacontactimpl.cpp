/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "metacontactimpl.h"
#include "manager.h"
#include <libqutim/debug.h>
#include <libqutim/tooltip.h>
#include <QApplication>
#include <libqutim/protocol.h>

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
			QString currentAvatar;
// 			for (int i = 0; i < m_contacts.size(); i++) {
// 				currentAvatar = m_contacts.at(i)->avatar();
// 				if (!currentAvatar.isEmpty())
// 					return currentAvatar;
// 			}
			return QString();
		}
		
		void MetaContactImpl::setName(const QString &name)
		{
			if (m_name == name)
				return;
			m_name = name;
			emit nameChanged(m_name);
		}
		
		Status MetaContactImpl::status() const
		{
			return m_status;
		}
		
		void MetaContactImpl::setTags(const QStringList &tags)
		{
			m_tags = tags;
			for (int i = 0; i < m_contacts.size(); i++)
				m_contacts.at(i)->setTags(tags);
			emit tagsChanged(tags);
		}

		bool MetaContactImpl::sendMessage(const Message &message)
		{
			//implement logic
			for (int i = 0; i < m_contacts.size(); i++) {
				if (m_contacts.at(i)->sendMessage(message))
					return true;
			}
			return false;
		}
		
		void MetaContactImpl::addContact(Contact *contact)
		{
			if (m_contacts.contains(contact) || (contact == this))
				return;
			
			QStringList contactTags = contact->tags();
			bool haveChanges = false;
			for (int i = 0; i < contactTags.size(); i++) {
				if (!m_tags.contains(contactTags.at(i))) {
					m_tags << contactTags.at(i);
					haveChanges = true;
				}
			}
			emit tagsChanged(m_tags);
			
			haveChanges = false;
			int index = qUpperBound(m_contacts.begin(), m_contacts.end(), contact, contactLessThan)
						- m_contacts.begin();
			m_contacts.insert(index, contact);
			MetaContact::addContact(contact);
			
			if (index == 0)
				resetStatus();
			if (m_contacts.size() == 1 || m_name.isEmpty())
				resetName();
			
			//setMenuOwner(contact); TODO, implement logic!
		}
		
		void MetaContactImpl::removeContact(Contact *contact)
		{
			int index = m_contacts.indexOf(contact);
			if (index == -1)
				return;
			m_contacts.removeAt(index);
			MetaContact::removeContact(contact);
			if (index == 0) {
				resetStatus();
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
				m_name = currentName;
				emit nameChanged(m_name);
			}
		}
		
		void MetaContactImpl::resetStatus()
		{
			if (m_contacts.isEmpty()) {
				if (m_status.type() == Status::Offline)
					return;
				m_status = Status();
				emit statusChanged(m_status);
				return;
			}
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
				QVariantHash hash = m_contacts.at(i)->status().extendedInfos();
				QVariantHash::const_iterator it = hash.constBegin();
				QVariantHash::const_iterator endit = hash.constEnd();
				for (; it != endit; it++) {
					if (!keys.contains(it.key())) {
						keys << it.key();
						m_status.setExtendedInfo(it.key(), it.value().toMap());
					}
				}
			}
			emit statusChanged(m_status);
		}
		
		void MetaContactImpl::onContactStatusChanged()
		{
			Contact *contact = qobject_cast<Contact*>(sender());
			int oldIndex = m_contacts.indexOf(contact);
			int index = qUpperBound(m_contacts.begin(), m_contacts.end(), contact, contactLessThan)
						- m_contacts.begin();
			if (index != oldIndex)
				m_contacts.move(oldIndex, index);
			if (index == 0 || oldIndex == 0)
				resetStatus();
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
				ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
				foreach (Contact *contact,m_contacts) {
					//small hack
					Status status = contact->status();
					QString icon = Status::iconName(status.type(),contact->account()->protocol()->id());
					QString body = QString("%1 (%2)").arg(contact->id(),contact->account()->name());
					event->appendField(contact->title(),body,icon);
					qApp->sendEvent(contact,event);
				}
			}			
			return qutim_sdk_0_3::Contact::event(ev);
		}


	}
}
