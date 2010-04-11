#ifndef VKONTAKTE_GLOBAL_H
#define VKONTAKTE_GLOBAL_H

#if defined(LIBVKONTAKTE_LIBRARY)
#  define LIBVKONTAKTE_EXPORT Q_DECL_EXPORT
#else
#  define LIBVKONTAKTE_EXPORT Q_DECL_IMPORT
#endif

#include <qutim/libqutim_global.h>
#include <qutim/status.h>

using namespace qutim_sdk_0_3;

enum VConnectionState {
	Invalid = -1,
	Connected,
	Connecting,
	Disconnected
};

VConnectionState LIBVKONTAKTE_EXPORT statusToState(Status::Type);
Status::Type LIBVKONTAKTE_EXPORT stateToStatus(VConnectionState state);

#endif // VKONTAKTE_GLOBAL_H
