#ifndef ROSTER_H
#define ROSTER_H

#include "snachandler.h"
#include "messageplugin.h"
#include "icqcontact.h"

class Roster : public SNACHandler
{
	Q_OBJECT
public:
	Roster(IcqAccount *account);
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac);
	inline QString groupId2Name(quint16 id) { return m_groups.value(id); }
private:
	void handleServerCListReply(OscarConnection *conn, const SNAC &snac);
	void handleUserOnline(OscarConnection *conn, const SNAC &snac);
	void handleUserOffline(OscarConnection *conn, const SNAC &snac);
	void handleMessage(OscarConnection *conn, const SNAC &snac);
	void sendRosterAck(OscarConnection *conn);
	enum State { ReceivingRoster, RosterReceived } m_state;
	IcqAccount *m_account;
	QMap<quint16, QString> m_groups;
	QMultiHash<QByteArray, MessagePlugin *> m_msg_plugins;
	QHash<QString, IcqContact *> m_contacts;
};

#endif // ROSTER_H
