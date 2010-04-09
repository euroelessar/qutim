#ifndef VKONTAKTE_GLOBAL_H
#define VKONTAKTE_GLOBAL_H
#include <qutim/libqutim_global.h>

#if defined(LIBVKONTAKTE_LIBRARY)
#  define LIBVKONTAKTE_EXPORT Q_DECL_EXPORT
#else
#  define LIBVKONTAKTE_EXPORT Q_DECL_IMPORT
#endif

using namespace qutim_sdk_0_3;

enum VConnectionState {
	Invalid = -1,
	Connected,
	Connecting,
	Disconnected
};

#endif // VKONTAKTE_GLOBAL_H
