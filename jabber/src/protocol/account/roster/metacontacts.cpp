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

#include "metacontacts.h"
#include <gloox/util.h>

namespace gloox
{
	const std::string XMLNS_METACONTACTS      = "storage:metacontacts";
	
	MetaContactsStorage::MetaContactsStorage(ClientBase *parent)
		: PrivateXML(parent), m_handler(0)
	{
	}
	
	MetaContactsStorage::~MetaContactsStorage()
	{
	}
	
	void MetaContactsStorage::storeMetaContacts(const MetaContactList &mcList)
	{
		Tag* s = new Tag("storage");
		s->addAttribute(XMLNS, XMLNS_METACONTACTS);
	
		MetaContactList::const_iterator it = mcList.begin();
		for (; it != mcList.end(); ++it) {
		  Tag* i = new Tag(s, "meta", "jid", (*it).jid);
		  i->addAttribute("tag", (*it).tag);
		  if ((*it).order >= 0)
			  i->addAttribute("order", util::int2string((*it).order));
		}
	
		storeXML(s, this);
	}

	void MetaContactsStorage::requestMetaContacts()
	{
		requestXML("storage", XMLNS_METACONTACTS, this);
	}

	void MetaContactsStorage::handlePrivateXML(const Tag *xml)
	{
		if( !xml )
		  return;
	
		MetaContactList mcList;
		const TagList &l = xml->children();
		TagList::const_iterator it = l.begin();
		for( ; it != l.end(); ++it )
		{
			if( (*it)->name() == "meta" )
			{
				const std::string &jid = (*it)->findAttribute("jid");
				const std::string &tag = (*it)->findAttribute("tag");
				const std::string &order = (*it)->findAttribute("order");
				
				if( !jid.empty() && !tag.empty() )
				{
					MetaContactListItem item;
					item.jid = jid;
					item.tag = tag;
					int orderInt = atoi(order.c_str());
					item.order = orderInt < 0 ? -1 : orderInt;
					mcList.push_back(item);
				}
			}
		}
		
		if(m_handler)
			m_handler->handleMetaContact(mcList);
	}

	void MetaContactsStorage::handlePrivateXMLResult(const std::string &uid, PrivateXMLResult pxResult)
	{
		(void)uid;
		(void)pxResult;
	}
}
