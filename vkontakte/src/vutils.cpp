#include "vkontakte_global.h"
#include "vkontakteprotocol.h"
#include <qutim/systeminfo.h>

VConnectionState statusToState(Status::Type status)
{
	VConnectionState state;
	switch (status) {
		case Status::Offline:
			state = Disconnected;
			break;
		case Status::Connecting:
			state = Connecting;
			break;
		default:
			state = Connected;
	};
	return state;
}

Status::Type stateToStatus(VConnectionState state)
{
	switch (state) {
		case Disconnected:
			return Status::Offline;
		case Connected:
			return Status::Online;
		case Connecting:
			return Status::Connecting;
		default:
			return Status::Offline;
	}
	
}

QDir getAvatarsDir()
{
	return QString("%1/avatars/%2")
			.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
			.arg(VkontakteProtocol::instance()->id());
}
