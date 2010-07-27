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
#include <gloox/client.h>
#include <gloox/disco.h>
#include <gloox/message.h>
#include <gloox/pubsubitem.h>
#include <qutim/objectgenerator.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/event.h>
#include <QCoreApplication>

namespace Jabber
{
	using namespace gloox;
	using namespace qutim_sdk_0_3;
	
	typedef QMap<Account*, JPersonEventSupport*> SupportMap;
	Q_GLOBAL_STATIC(SupportMap, supportMap);
	
	JPersonalEventFilterFactory::JPersonalEventFilterFactory()
	{
	}

	JPersonalEventFilterFactory::~JPersonalEventFilterFactory()
	{
	}

	MessageFilter *JPersonalEventFilterFactory::create(Account *account,
													   const JabberParams &params,
													   MessageSession *session)
	{
		Q_UNUSED(params);
		JPersonEventSupport *support = supportMap()->value(account);
		return new JPersonalEventFilter(support, session);
	}

	JPersonalEventFilter::JPersonalEventFilter(JPersonEventSupport *support, MessageSession *session) :
			MessageFilter(session), m_support(support)
	{
	}

	JPersonalEventFilter::~JPersonalEventFilter()
	{
	}

	void JPersonalEventFilter::decorate(gloox::Message &msg)
	{
		Q_UNUSED(msg);
	}

	void JPersonalEventFilter::filter(gloox::Message &msg)
	{
		const PubSub::Event *event = msg.findExtension<PubSub::Event>(ExtPubSubEvent);
		if (event) {
			PubSub::Event::ItemOperationList items = event->items();
			foreach (const PubSub::Event::ItemOperation *item, items) {
				if (!item->payload || item->payload->children().empty())
					continue;
				m_support->handleTag(msg.from().bare(), item->payload->children().front());
			}
		}
	}
	
	JPersonEventSupport::JPersonEventSupport() : m_account(0), m_manager(0), m_eventId(0)
	{
	}
	
	JPersonEventSupport::~JPersonEventSupport()
	{
		supportMap()->remove(m_account);
		delete m_manager;
	}
	
	void JPersonEventSupport::init(Account *account, const JabberParams &params)
	{
		supportMap()->insert(account, this);
		m_account = account;
		Client *client = params.item<Client>();
		client->registerStanzaExtension(new PubSub::Event(reinterpret_cast<Tag*>(0)));
		m_manager = new PubSub::Manager(client);
		account->installEventFilter(this);
		m_eventId = qutim_sdk_0_3::Event::registerType("jabber-personal-event");
		foreach (const ObjectGenerator *ext, moduleGenerators<PersonEventConverter>()) {
			PersonEventConverter *converter = ext->generate<PersonEventConverter>();
			m_converters.insert(converter->name(), converter);
			std::string feature = converter->feature();
			client->disco()->addFeature(feature);
			client->disco()->addFeature(feature + "+notify");
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
				if (needSet && !!(converter = m_converters.value(name))) {
					QVariantMap data = customEvent->at<QVariantMap>(1);
					PubSub::ItemList items;
					Tag *tag = new Tag("item");
					tag->addChild(converter->toXml(data));
					items.push_back(new  PubSub::Item(tag));
					m_manager->publishItem(JID(), converter->feature(), items, 0, this);
				}
			}
		}
		return false;
	}
	
	void JPersonEventSupport::handleTag(const std::string &jid, gloox::Tag *tag)
	{
		QString unicodeJid = QString::fromStdString(jid);
		QObject *receiver = 0;
		if (unicodeJid == m_account->id())
			receiver = m_account;
		else if (ChatUnit *unit = m_account->getUnit(unicodeJid, false))
			receiver = qobject_cast<Contact*>(unit) ? unit : 0;
		if (receiver) {
			QString name = QString::fromStdString(tag->name());
			if (PersonEventConverter *converter = m_converters.value(name)) {
				QVariantMap data = converter->fromXml(tag);
				qutim_sdk_0_3::Event ev(m_eventId, name, data, false);
				qApp->sendEvent(receiver, &ev);
			}
		}
	}
}
