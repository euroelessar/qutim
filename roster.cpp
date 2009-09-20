#include "roster.h"

Roster::Roster()
{
	m_infos << SNACInfo(ListsFamily, ListsList)
			<< SNACInfo(BuddyFamily, UserOnline)
			<< SNACInfo(BuddyFamily, UserOffline);
	m_state = ReceivingRoster;
}

void Roster::handleSNAC(OscarConnection *conn, const SNAC &sn)
{
	switch((sn.family() << 16) | sn.subtype())
	{
		// Server sends contactlist
		case ListsFamily << 16 | ListsList:
			handleServerCListReply(conn, sn);
		break;
		case BuddyFamily << 16 | UserOnline:
			handleUserOnline(conn, sn);
		break;
		case BuddyFamily << 16 | UserOffline:
			handleUserOffline(conn, sn);
		break;
	}
}

void Roster::handleServerCListReply(OscarConnection *conn, const SNAC &sn)
{
	Q_UNUSED(conn);
	if(!(sn.flags() & 0x0001))
		m_state = RosterReceived;
	quint8 version = sn.readSimple<quint8>();
	quint16 count = sn.readSimple<quint16>();
	bool is_last = !(sn.flags() & 0x0001);
	qDebug("SSI: number of entries is %u, version is %u", (uint)count, (uint)version);
	for(uint i = 0; i < count; i++)
	{
		QString record_name = sn.readString<quint16>();
		quint16 group_id = sn.readSimple<quint16>();
		quint16 item_id = sn.readSimple<quint16>();
		quint16 item_type = sn.readSimple<quint16>();
		QByteArray tlv_data = sn.readData<quint16>();
		const TLVMap tlvs = TLV::parseByteArray(tlv_data);
		switch(item_type)
		{
		case SsiBuddy: {
			// record name contains uin, while item id contains some random value
			QString nick;
			QString comment;
			bool not_auth = false;
			if(tlvs.contains(0x0131))
				nick = tlvs[0x0131].value<QString>();
			if(tlvs.contains(0x013c))
				comment = tlvs[0x013c].value<QString>();
			if(tlvs.contains(0x0066))
				not_auth = true;
			qDebug() << nick;
			break; }
		case SsiGroup:
			if(group_id > 0)
			{
				// record name contains name of group
				m_groups.insert(group_id, record_name);
			}
			else
			{
			}
			break;
		}
		QString buf;
		{
			QTextStream out(&buf, QIODevice::WriteOnly);
			out << record_name << " "
					<< group_id << " " << item_id << " "
					<< item_type << " (";
			bool first = true;
			foreach(const TLV &tlv, tlvs)
			{
				if(!first)
					out << ", ";
				else
					first = false;
				out << "0x" << QString::number(tlv.type(), 16);
			}
			out << ")";
		}
		qDebug("%s", qPrintable(buf));
	}
	qDebug() << "is_last" << is_last;
	if(is_last)
	{
		quint32 last_info_update = sn.readSimple<quint32>();
		qDebug() << last_info_update;
		conn->setProperty("SrvLastUpdate", last_info_update);
		sendRosterAck(conn);
		conn->finishLogin();
	}
	qDebug() << m_groups;
}

void Roster::handleUserOnline(OscarConnection *conn, const SNAC &snac)
{
	QString uin = snac.readData<quint8>();
	quint16 warning_level = snac.readSimple<quint16>();
	TLVMap tlvs = snac.readTLVChain<quint16>();
	if(tlvs.contains(0x000c))
	{
		DataUnit data(tlvs.value(0x000c));
		DirectConnectionInfo info =
		{
			QHostAddress(data.readSimple<quint32>()),
			data.readSimple<quint32>(),
			data.readSimple<quint8>(),
			data.readSimple<quint16>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>(),
			data.readSimple<quint32>()
		};
	}
	qDebug("Handle UserOnline %02X %02X, %s", (int)snac.family(), (int)snac.subtype(), qPrintable(uin));
	qDebug() << tlvs.keys();
	TLV tlv06 = tlvs.value(0x0006);
	qDebug("%s", qPrintable(QString::number(tlv06.value<quint32>(), 16)));
//	bool status_present = tlv06.type() == 0x0006;
}

void Roster::handleUserOffline(OscarConnection *conn, const SNAC &snac)
{
}

void Roster::sendRosterAck(OscarConnection *conn)
{
	SNAC snac(ListsFamily, ListsGotList);
	conn->send(snac);
	qDebug("Send Roster Ack, SNAC %02X %02X", (int)snac.family(), (int)snac.subtype());
}
