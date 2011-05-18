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

#ifndef METACONTACTS_H
#define METACONTACTS_H

//#include <gloox/privatexml.h>

//namespace gloox
//{
//extern const std::string XMLNS_METACONTACTS;

//struct MetaContactListItem
//{
//	std::string jid;
//	std::string tag;
//	int order;
//};

//typedef std::list<MetaContactListItem> MetaContactList;

//class MetaContactHandler
//{
//public:
//	virtual ~MetaContactHandler() {}
//	virtual void handleMetaContact(const MetaContactList &mcList) = 0;
//};

//class MetaContactsStorage : public PrivateXML, public PrivateXMLHandler
//{
//public:
//	MetaContactsStorage(ClientBase *client);
//	virtual ~MetaContactsStorage();

//	void storeMetaContacts(const MetaContactList &mcList);

//	void requestMetaContacts();

//	void registerMetaContactHandler(MetaContactHandler* mch)
//	{ m_handler = mch; }

//	void removeMetaContactHandler()
//	{ m_handler = 0; }

//	// reimplemented from PrivateXMLHandler
//	virtual void handlePrivateXML(const Tag *xml);

//	// reimplemented from PrivateXMLHandler
//	virtual void handlePrivateXMLResult(const std::string &uid, PrivateXMLResult pxResult);
//private:
//	MetaContactHandler *m_handler;
//};
//}

#endif // METACONTACTS_H
