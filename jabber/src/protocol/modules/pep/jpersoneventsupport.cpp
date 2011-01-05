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

#include "jpersoneventsupport.h"
#include <jreen/client.h>
//#include <jreen/disco.h>
//#include <jreen/message.h>
//#include <jreen/pubsubitem.h>
#include <qutim/objectgenerator.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/event.h>
#include <QCoreApplication>
#include <protocol/account/roster/jcontact.h>
#include <QDebug>

namespace Jabber
{
	using namespace jreen;
	using namespace qutim_sdk_0_3;
	
//	typedef QMap<Account*, JPersonEventSupport*> SupportMap;
//	Q_GLOBAL_STATIC(SupportMap, supportMap);
	
//	JPersonalEventFilterFactory::JPersonalEventFilterFactory()
//	{
//	}

//	JPersonalEventFilterFactory::~JPersonalEventFilterFactory()
//	{
//	}

//	MessageFilter *JPersonalEventFilterFactory::create(Account *account,
//													   const JabberParams &params,
//													   MessageSession *session)
//	{
//		Q_UNUSED(params);
//		JPersonEventSupport *support = supportMap()->value(account);
//		return new JPersonalEventFilter(support, session);
//	}

//	JPersonalEventFilter::JPersonalEventFilter(JPersonEventSupport *support, MessageSession *session) :
//			MessageFilter(session), m_support(support)
//	{
//	}

//	JPersonalEventFilter::~JPersonalEventFilter()
//	{
//	}

//	void JPersonalEventFilter::decorate(gloox::Message &msg)
//	{
//		Q_UNUSED(msg);
//	}

//	void JPersonalEventFilter::filter(gloox::Message &msg)
//	{
//		const PubSub::Event *event = msg.findExtension<PubSub::Event>(ExtPubSubEvent);
//		if (event) {
//			PubSub::Event::ItemOperationList items = event->items();
//			foreach (const PubSub::Event::ItemOperation *item, items) {
//				if (!item->payload || item->payload->children().empty())
//					continue;
//				m_support->handleTag(msg.from().bare(), item->payload->children().front());
//			}
//		}
//	}
	
	JPersonEventSupport::JPersonEventSupport() : m_account(0), m_manager(0), m_eventId(0)
	{
	}
	
	JPersonEventSupport::~JPersonEventSupport()
	{
//		supportMap()->remove(m_account);
//		delete m_manager;
	}
	
	void JPersonEventSupport::init(Account *account, const JabberParams &params)
	{
//		supportMap()->insert(account, this);
		m_account = account;
		Client *client = params.item<Client>();
//		client->registerStanzaExtension(new PubSub::Event(reinterpret_cast<Tag*>(0)));
		m_manager = new PubSub::Manager(client);
		connect(m_manager, SIGNAL(eventReceived(jreen::PubSub::Event::Ptr,jreen::JID)),
				this, SLOT(onEventReceived(jreen::PubSub::Event::Ptr,jreen::JID)));
		account->installEventFilter(this);
		m_eventId = qutim_sdk_0_3::Event::registerType("jabber-personal-event");
		foreach (const ObjectGenerator *ext, ObjectGenerator::module<PersonEventConverter>()) {
			PersonEventConverter *converter = ext->generate<PersonEventConverter>();
			m_converters.insert(converter->entityType(), converter);
			m_manager->addEntityType(converter->entityType());
		}
	}
	
	bool JPersonEventSupport::eventFilter(QObject *obj, QEvent *ev)
	{
		if (ev->type() == qutim_sdk_0_3::Event::eventType()) {
			qutim_sdk_0_3::Event *customEvent = static_cast<qutim_sdk_0_3::Event*>(ev);
			if (customEvent->id == m_eventId && obj == m_account) {
				QString name = customEvent->at<QString>(0);
				bool needSet = customEvent->at<bool>(2);
				PersonEventConverter *converter = 0;
				foreach (PersonEventConverter *conv, m_converters) {
					if (conv->name() == name) {
						converter = conv;
						break;
					}
				}

				if (needSet && converter) {
					QVariantHash data = customEvent->at<QVariantHash>(1);
					QList<jreen::StanzaExtension::Ptr> items;
					items << converter->convertTo(data);
					m_manager->publishItems(items, jreen::JID());
				}
			}
		}
		return false;
	}
	
	void JPersonEventSupport::onEventReceived(const jreen::PubSub::Event::Ptr &event, const jreen::JID &from)
	{
		QObject *receiver = 0;
		JContact *contact = 0;
		if (from.bare() == m_account->id()) {
			receiver = m_account;
		} else if (ChatUnit *unit = m_account->getUnit(from.bare(), false)) {
			contact = qobject_cast<JContact*>(unit);
			receiver = contact;
		}
		if (!receiver)
			return;
		const QList<jreen::StanzaExtension::Ptr> items = event->items();
		for (int i = 0; i < items.size(); i++) {
			if (PersonEventConverter *converter = m_converters.value(items[i]->extensionType())) {
				QVariantHash data = converter->convertFrom(items[i]);
				QString name = converter->name();
				if (contact) {
					if (!data.isEmpty())
						contact->setExtendedInfo(name, data);
					else
						contact->removeExtendedInfo(name);
				}
				qutim_sdk_0_3::Event ev(m_eventId, name, data, false);
				qApp->sendEvent(receiver, &ev);
			}
		}
	}

}
