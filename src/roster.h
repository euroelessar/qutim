/****************************************************************************
 *  roster.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef ROSTER_H
#define ROSTER_H

#include <QQueue>
#include "snachandler.h"
#include "messageplugin.h"
#include "icqcontact.h"

class SSIItem;

class Roster : public SNACHandler
{
	Q_OBJECT
public:
	Roster(IcqAccount *account);
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac);
	inline QString groupId2Name(quint16 id) { return m_groups.value(id); }
	IcqContact *contact(const QString &uin) { return m_contacts.value(uin); }
	void sendMessage(OscarConnection *conn, const QString &id, const QString &message);
	void sendAuthResponse(OscarConnection *conn, const QString &id, const QString &message, bool auth = true);
	void sendAddGroupRequest(OscarConnection *conn, const QString &name, quint16 group_id = 0);
	void sendRemoveGroupRequest(OscarConnection *conn, quint16 id);
	IcqContact *sendAddContactRequest(OscarConnection *conn, const QString &contact_id, const QString &contact_name, quint16 group_id);
	void sendRemoveContactRequst(OscarConnection *conn, const QString &contact_id);
private:
	enum ModifingType {
		mt_add = ListsAddToList,
		mt_add_modify = ListsList,
		mt_modify = ListsUpdateGroup,
		mt_remove =  ListsRemoveFromList
	};
	struct SSIItem
	{
		SSIItem():
			group_id(0), item_id(0)
		{}
		SSIItem(const SNAC &snac);
		~SSIItem(){}
		QString toString() const;
		QString record_name;
		quint16 group_id;
		quint16 item_id;
		quint16 item_type;
		TLVMap tlvs;
	};
	struct SSIHistoryItem
	{
		SSIHistoryItem(const SSIItem &item_, ModifingType type_):
			item(item_), type(type_)
		{
		}
		SSIItem item;
		ModifingType type;
	};
	enum State { ReceivingRoster, RosterReceived } m_state;
	void handleServerCListReply(OscarConnection *conn, const SNAC &snac);
	void handleSSIItem(const SSIItem &item, ModifingType type);
	void handleAddModifyCLItem(const SSIItem &item, ModifingType type);
	void handleRemoveCLItem(const SSIItem &item);
	void removeContact(IcqContact *contact);
	void handleSSIServerAck(OscarConnection *conn, const SNAC &sn);
	void handleUserOnline(OscarConnection *conn, const SNAC &snac);
	void handleUserOffline(OscarConnection *conn, const SNAC &snac);
	void handleMessage(OscarConnection *conn, const SNAC &snac);
	void handleError(OscarConnection *conn, const SNAC &snac);
	void handleMetaInfo(OscarConnection *conn, const SNAC &snac);
	void sendRosterAck(OscarConnection *conn);
	void sendOfflineMessagesRequest(OscarConnection *conn) { sendMetaInfoRequest(conn, 0x003C); }
	void sendMetaInfoRequest(OscarConnection *conn, quint16 type);
	void sendCLModifyStart(OscarConnection *conn);
	void sendCLModifyEnd(OscarConnection *conn);
	void sendCLOperator(OscarConnection *conn, const SSIItem &item, quint16 operation);
	quint16 generateGroupID();
	IcqAccount *m_account;
	QMap<quint16, QString> m_groups;
	QMultiHash<Capability, MessagePlugin *> m_msg_plugins;
	QHash<QString, IcqContact *> m_contacts;
	QHash<QString, IcqContact *> m_not_in_list;
	QQueue<SSIHistoryItem> m_ssi_history;
};

#endif // ROSTER_H
