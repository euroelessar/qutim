#include "chatnavigation.h"
#include "chatconnection.h"
#include "oscarchat.h"
#include "icqaccount.h"

namespace qutim_sdk_0_3 {
namespace oscar {


ChatNavigation::ChatNavigation(IcqAccount *account, QObject *parent) :
	AbstractConnection(account, parent)
{
	m_infos << SNACInfo(ServiceFamily, ServerRedirectService)
			<< SNACInfo(ChatNavigationFamily, NavigationError)
			<< SNACInfo(ChatNavigationFamily, NavigationInfo);
	registerHandler(this);
}

void ChatNavigation::processNewConnection()
{
	AbstractConnection::processNewConnection();
	FLAP flap(0x01);
	flap.append<quint32>(0x01);
	flap.appendTLV<QByteArray>(0x0006, m_cookie);
	m_cookie.clear();
	send(flap);
}

void ChatNavigation::processCloseConnection()
{
	AbstractConnection::processCloseConnection();
}

void ChatNavigation::handleSNAC(AbstractConnection *conn, const SNAC &snac)
{
	if (conn == account()->connection()) {
		switch ((snac.family() << 16) | snac.subtype()) {
		case ServiceFamily << 16 | ServerRedirectService: {
			TLVMap tlvs = snac.read<TLVMap>();
			quint16 id = tlvs.value(0x0D).read<quint16>();
			debug() << Q_FUNC_INFO << "service" << id << tlvs;
			if (id == ChatNavigationFamily) {
				QList<QByteArray> list = tlvs.value(0x05).data().split(':');
				m_cookie = tlvs.value(0x06).data();
				debug() << tlvs.value(0x05).data();
				socket()->connectToHost(list.at(0), list.value(1, "5190").toInt());
			} else if (id == ChatFamily) {
				debug() << Q_FUNC_INFO << "join reply" << snac.id();
				new ChatConnection(tlvs, account());
			}
			break;
		}
		case ServiceFamily << 16 | ServiceServerAsksServices: {
			// Requesting navigation service
			SNAC snac(ServiceFamily, ServiceClientNewService);
			snac.append<quint16>(ChatNavigationFamily);
			conn->send(snac);
			break;
		}
		}
		return;
	}

	Q_ASSERT(this == conn);
	AbstractConnection::handleSNAC(this, snac);
	snac.resetState();
	switch ((snac.family() << 16) | snac.subtype()) {
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		SNAC snac(ServiceFamily, ServiceClientReady);
		snac.append(QByteArray::fromHex(
				"0001 0004 0110 164f" // ServiceFamily
				"000d 0001 0110 164f")); // ChatNavigationFamily
		send(snac);
		setState(Connected);

		snac.reset(ChatNavigationFamily, RequestChatRights);
		send(snac);
		break;
	}
	case ChatNavigationFamily << 16 | NavigationInfo : {
		debug() << Q_FUNC_INFO << snac.data().toHex();
		TLVMap tlvs = snac.read<TLVMap>();
		debug() << Q_FUNC_INFO << tlvs.keys() << tlvs.values(3).size();
		if (tlvs.contains(2)) {
			maxCurrentRooms = tlvs.value<quint8>(2);
			debug() << maxCurrentRooms;
		}
		foreach (const TLV &exchangeTLV, tlvs.values(3)) {
			debug() << "hex" << exchangeTLV.data().toHex();
			quint16 id = exchangeTLV.read<quint16>();
			TLVMap exchangeTLVs = exchangeTLV.read<TLVMap, quint16>();

			if (tlvs.contains(2)) { // means this is an answer on SNAC(ChatNavigationFamily, RequestChatRights)
				// Request information for the exchange
				SNAC infoSnac(ChatNavigationFamily, RequestExchangeInfo);
				infoSnac.append<quint16>(id);
				send(infoSnac);
			}
			debug() << "Exchange:" << id << exchangeTLVs;
		}
		foreach (const TLV &roomTLV, tlvs.values(4)) {
			// This code is never called
			debug() << "hex" << roomTLV.data().toHex();
			RoomId id = roomTLV.read<RoomId>();
			quint8 detailLevel = roomTLV.read<quint8>();
			TLVMap roomTLVs = roomTLV.read<TLVMap, quint16>();
//			joinRoom(id);
			debug() << "Room:" << id.cookie.toHex() << id.instance << detailLevel << roomTLVs;
			OscarChat *chat = new OscarChat(id, this);
			chat->join();
		}

		// some test requests
		if (tlvs.contains(2)) { // means this is an answer on SNAC(ChatNavigationFamily, RequestChatRights)
			// Request information for exchange 4
			// Server answers on that but does not send a room list.
			SNAC infoSnac(ChatNavigationFamily, RequestExchangeInfo);
			infoSnac.append<quint16>(4);
			send(infoSnac);

			// trying to create a new room...
			createRoom("qutIM talks");
		}

		break;
	}
	case ChatNavigationFamily << 16 | NavigationError: {
		ProtocolError error(snac);
		debug() << QString("Chat navigation error (%1, %2): %3")
				   .arg(error.code(), 2, 16)
				   .arg(error.subcode(), 2, 16)
				   .arg(error.errorString());
		break;
	}
	}
}

void ChatNavigation::createRoom(const QString &name)
{
	SNAC snac(ChatNavigationFamily, CreateRoom);
	snac.append<quint16>(4); // exchange
	snac.append<quint8>("create");
	snac.append<quint16>(0); // instance
	snac.append<quint8>(1); // detail level
	TLVMap tlvs;
	tlvs.insert<QString>(0x00d6, "us-ascii");
	QString roomName = name;
	if (roomName.isEmpty()) {
		roomName = QLatin1String("Chat ");
		roomName += QString::number((quint64(qrand()) << 32) | quint64(qrand()));
	}
	tlvs.insert<QString>(0x00d3, roomName);
	tlvs.insert<QString>(0x00d7, "en");
	snac.append<quint16>(tlvs);
	send(snac);
}

void ChatNavigation::joinRoom(const RoomId &id)
{
	SNAC snac(ServiceFamily, ServiceClientNewService);
	debug() << Q_FUNC_INFO << snac.id() << id.cookie.toHex();
	snac.append<quint16>(ChatFamily);
	snac.appendTLV(0x0001, id);
	account()->connection()->send(snac);
}

} // namespace oscar
} // namespace qutim_sdk_0_3
