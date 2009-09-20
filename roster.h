#ifndef ROSTER_H
#define ROSTER_H

#include "snachandler.h"

class Roster : public SNACHandler
{
public:
    Roster();
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac);
private:
	void handleServerCListReply(OscarConnection *conn, const SNAC &snac);
	void handleUserOnline(OscarConnection *conn, const SNAC &snac);
	void handleUserOffline(OscarConnection *conn, const SNAC &snac);
	void sendRosterAck(OscarConnection *conn);
	enum State { ReceivingRoster, RosterReceived } m_state;
	QMap<quint16, QString> m_groups;
};

#endif // ROSTER_H
