/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "../../account/roster/jcontact.h"
#include <QDebug>

namespace Jabber
{
	using namespace Jreen;
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
//		const PubSub::Event *event = msg.payload<PubSub::Event>(ExtPubSubEvent);
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
	
	void JPersonEventSupport::init(Account *account)
	{
//		supportMap()->insert(account, this);
		m_account = account;
		m_manager = qobject_cast<PubSub::Manager*>(account->property("pubSubManager"));
//		client->registerPayload(new PubSub::Event(reinterpret_cast<Tag*>(0)));
		connect(m_manager, SIGNAL(eventReceived(Jreen::PubSub::Event::Ptr,Jreen::JID)),
				this, SLOT(onEventReceived(Jreen::PubSub::Event::Ptr,Jreen::JID)));
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
					QList<Jreen::Payload::Ptr> items;
					items << converter->convertTo(data);
					m_manager->publishItems(items, Jreen::JID());
				}
			}
		}
		return false;
	}
	
	void JPersonEventSupport::onEventReceived(const Jreen::PubSub::Event::Ptr &event, const Jreen::JID &from)
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
		const QList<Jreen::Payload::Ptr> items = event->items();
		for (int i = 0; i < items.size(); i++) {
			if (PersonEventConverter *converter = m_converters.value(items[i]->payloadType())) {
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

