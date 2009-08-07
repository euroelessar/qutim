#ifndef LIBQUTIM_GLOBAL_H
#define LIBQUTIM_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QObject>

#if defined(LIBQUTIM_LIBRARY)
#  define LIBQUTIM_EXPORT Q_DECL_EXPORT
#else
#  define LIBQUTIM_EXPORT Q_DECL_IMPORT
#endif

#define QUTIM_VERSION_STR "0.2.60.0"
#define QUTIM_VERSION 0x00023c00
#define QUTIM_VERSION_CHECK(major, minor, secminor, patch) ((major<<24)|(minor<<16)|(secminor<<8)|(patch))

namespace qutim_sdk_0_3
{
	LIBQUTIM_EXPORT const char *qutimVersion();
}

#endif // LIBQUTIM_GLOBAL_H
