#include "chatnavigation.h"
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
			if (id == ChatNavigationFamily) {
				QList<QByteArray> list = tlvs.value(0x05).data().split(':');
				m_cookie = tlvs.value(0x06).data();
				socket()->connectToHost(list.at(0), list.size() > 1 ? atoi(list.at(1).constData()) : 5190);
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
		//debug() << snac.data().toHex();
		TLVMap tlvs = snac.read<TLVMap>();
		if (tlvs.contains(2)) {
			maxCurrentRooms = tlvs.value<quint8>(2);
			debug() << maxCurrentRooms;
		}
		foreach (const TLV &exchangeTLV, tlvs.values(3)) {
			quint16 id = exchangeTLV.read<quint16>();
			TLVMap exchangeTLVs = exchangeTLV.read<TLVMap>();

			if (tlvs.contains(2)) { // means this is an answer on SNAC(ChatNavigationFamily, RequestChatRights)
				// Request information for the exchange
				SNAC snac(ChatNavigationFamily, RequestExchangeInfo);
				snac.append<quint16>(id);
				send(snac);
			}
			debug() << "Exchange:" << id << exchangeTLVs;
		}
		foreach (const TLV &roomTLV, tlvs.values(4)) {
			// This code is never called
			quint16 id = roomTLV.read<quint16>();
			quint8 detailLevel = roomTLV.read<quint8>();
			TLVMap roomTLVs = roomTLV.read<TLVMap>();
			debug() << "Room:" << id << detailLevel << roomTLVs;
		}

		// some test requests
		if (tlvs.contains(2)) { // means this is an answer on SNAC(ChatNavigationFamily, RequestChatRights)
			// Request information for exchange 4
			// Server answers on that but does not send a room list.
			SNAC snac(ChatNavigationFamily, RequestExchangeInfo);
			snac.append<quint16>(4);
			send(snac);

			// trying to create a new room... Server replies with "Incorrect SNAC format" error
			snac.reset(ChatNavigationFamily, CreateRoom);
			snac.append<quint16>(4); // exchange
			snac.append<quint8>(0); // cookie
			snac.append<quint16>(-1); // instance
			snac.append<quint8>(0x01); // detailLevel
			TLVMap tlvs;
			tlvs.insert(0x00d3, "Test");
			//tlvs.insert(0x00d6, "us-ascii");
			//tlvs.insert(0x00d7, "en");
			snac.append<quint16>(tlvs);
			send(snac);
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


} // namespace oscar
} // namespace qutim_sdk_0_3
