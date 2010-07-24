/****************************************************************************
 *
  * This file is part of qutIM
 *
  * Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
  *                                                                        *
  *  This file is part of free software; you can redistribute it and/or    *
  *  modify it under the terms of the GNU General Public License as        *
  *  published by the Free Software Foundation; either version 2 of the    *
  *  License, or (at your option) any later version.                       *
  *                                                                        *
 ***************************************************************************
 ****************************************************************************/

#ifndef JPERSONEVENTSUPPORT_H
#define JPERSONEVENTSUPPORT_H

#include <gloox/pubsub.h>
#include <gloox/pubsubmanager.h>
#include <gloox/pubsubresulthandler.h>
#include <gloox/pubsubevent.h>
#include <gloox/messagefilter.h>
#include "../../../sdk/jabber.h"

namespace Jabber
{
	class JPersonEventSupport;
	
	class JPersonalEventFilterFactory : public QObject, public MessageFilterFactory
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::MessageFilterFactory)
	public:
		JPersonalEventFilterFactory();
		virtual ~JPersonalEventFilterFactory();
		virtual gloox::MessageFilter *create(qutim_sdk_0_3::Account *account,
											 const JabberParams &params,
											 gloox::MessageSession *session);
		
	};

	class JPersonalEventFilter : public gloox::MessageFilter
	{
	public:
		JPersonalEventFilter(JPersonEventSupport *support, gloox::MessageSession *session);
		~JPersonalEventFilter();
		virtual void decorate(gloox::Message &msg);
		virtual void filter(gloox::Message &msg);
	private:
		JPersonEventSupport *m_support;
	};
	
	class JPersonEventSupport :
			public QObject, public JabberExtension, public gloox::PubSub::ResultHandler
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JabberExtension)
	public:
		JPersonEventSupport();
		virtual ~JPersonEventSupport();
		virtual void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
		virtual bool eventFilter(QObject *obj, QEvent *ev);
		void handleTag(const std::string &jid, gloox::Tag *tag);
		
		// wow... reimplemented from gloox::PubSub::ResultHandler
        virtual void handleItem(const gloox::JID &, const std::string &, const gloox::Tag *) {}
        virtual void handleItems(const std::string &, const gloox::JID &, const std::string &,
								 const gloox::PubSub::ItemList &,
								 const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleItemPublication(const std::string &, const gloox::JID &,
										   const std::string &, const gloox::PubSub::ItemList &,
										   const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleItemDeletion(const std::string &, const gloox::JID &,
										const std::string &, const gloox::PubSub::ItemList &,
										const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleSubscriptionResult(const std::string &, const gloox::JID &,
											  const std::string &, const std::string &,
											  const gloox::JID &, const gloox::PubSub::SubscriptionType,
											  const gloox::Error  *error = 0) { Q_UNUSED(error); }
		virtual void handleUnsubscriptionResult(const std::string &, const gloox::JID &,
												const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleSubscriptionOptions(const std::string &, const gloox::JID &, const gloox::JID &,
											   const std::string &, const gloox::DataForm *,
											   const std::string & = gloox::EmptyString,
											   const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleSubscriptionOptionsResult(const std::string &, const gloox::JID &,
													 const gloox::JID &, const std::string &,
													 const std::string & = gloox::EmptyString,
													 const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleSubscribers(const std::string &, const gloox::JID &,
									   const std::string &, const gloox::PubSub::SubscriberList *,
									   const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleSubscribersResult(const std::string &, const gloox::JID &,
											 const std::string &, const gloox::PubSub::SubscriberList *,
											 const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleAffiliates(const std::string &, const gloox::JID &, const std::string &,
									  const gloox::PubSub::AffiliateList *,
									  const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleAffiliatesResult(const std::string &, const gloox::JID &,
											const std::string &, const gloox::PubSub::AffiliateList *,
											const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleNodeConfig(const std::string &, const gloox::JID &, const std::string &,
									  const gloox::DataForm *, const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleNodeConfigResult(const std::string &, const gloox::JID &, const std::string &,
											const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleNodeCreation(const std::string &, const gloox::JID &, const std::string &,
										const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleNodeDeletion(const std::string &, const gloox::JID &, const std::string &,
										const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleNodePurge(const std::string &, const gloox::JID &, const std::string &,
									 const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleSubscriptions(const std::string &, const gloox::JID &,
										 const gloox::PubSub::SubscriptionMap &,
										 const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleAffiliations(const std::string &, const gloox::JID &,
										const gloox::PubSub::AffiliationMap &,
										const gloox::Error  *error = 0) { Q_UNUSED(error); }
        virtual void handleDefaultNodeConfig(const std::string &, const gloox::JID &, const gloox::DataForm *,
											 const gloox::Error  *error = 0) { Q_UNUSED(error); }
	private:
		qutim_sdk_0_3::Account *m_account;
		gloox::PubSub::Manager *m_manager;
		quint16 m_eventId;
		QMap<QString, PersonEventConverter*> m_converters;
	};
}

#endif // JPERSONEVENTSUPPORT_H
