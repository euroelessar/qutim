#ifndef OSCAR_CHATNAVIGATION_H
#define OSCAR_CHATNAVIGATION_H

#include "icq_global.h"
#include "connection.h"

namespace qutim_sdk_0_3 {
namespace oscar {

class IcqAccount;

enum ChatNavigationSubtype
{
	NavigationError = 0x0001,
	RequestChatRights = 0x0002,
	RequestExchangeInfo = 0x0003,
	SearchForRoom = 0x0007,
	CreateRoom = 0x0008,
	NavigationInfo = 0x0009
};

enum CreationPermission
{
	// creation of rooms is not allowed in
	// this room or exchange
	CreationNotAllowed,
	// creation of child rooms is allowed in
	// this room (does not apply to exchanges)
	RoomCreationAllowed,
	// creation of rooms is allowed in some
	// of the rooms in this exchange (does
	// not apply to rooms)
	ExchangeCreationAllowed
};

enum ChatFlags
{
	// the room (itself) is evilable via
	// resource evil.  If not evilable,
	// ROOM_GROSS_EVIL and ROOM_NET_EVIL
	// do not apply
	Evitable = 0x01,
	// the room is present for
	// navigational purposes only and it is not
	// possible to join it. (this is how AOL's
	// categories would be implemented).
	// Note, this implies MAX_OCCUPANCY 0 and
	// no INSTANCING_ALLOWED, but only for
	// this room (ie. not inherited)
	NavigationOnly = 0x02,
	// new instances of a room will be
	// created if it gets too full
	InstancingAllowed = 0x04,
	// a room will allow a client to get
	// it's occupant list w/o requiring
	// the requester to be in the room.
	OccupantPeekAllowed = 0x08,
	// TLV_TAGS WHISPER_TO_USER is allowed
	// in this room
	WhisperAllowed = 0x10,
	// users in this room may evil one another
	// using CHAT snac EVIL_REQUEST
	EvilAllowed = 0x20,
	// users in this room may send
	// CLIENT_EVENT snacs
	EventsAllowed = 0x40
};

struct ExchangeDescription
{
	quint16 id;
	//QString language;
	QString name;
	quint8 maxCurrentRooms;
	bool isBrowsable;
	quint8 maxRoomNameLength;
	quint16 maxMessageLength;
	CreationPermission permission;
	quint16 maxOccupancy;
	ChatFlags flags;
};

class ChatNavigation : public AbstractConnection
{
	Q_OBJECT
public:
	ChatNavigation(IcqAccount *account, QObject *parent);
protected:
	virtual void processNewConnection();
	virtual void processCloseConnection();
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private:
	QByteArray m_cookie;
	quint8 maxCurrentRooms;
	QHash<quint16, ExchangeDescription> m_exchanges;
};

} // namespace oscar
} // namespace qutim_sdk_0_3

#endif // OSCAR_CHATNAVIGATION_H
