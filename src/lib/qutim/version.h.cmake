/****************************************************************************
 *  version.h
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef _QUTIM_VERSION_H_
#define _QUTIM_VERSION_H_

#include "libqutim_global.h"

#define QUTIM_VERSION_STRING "${CMAKE_QUTIM_VERSION_STRING}"
#define QUTIM_VERSION_MAJOR ${CMAKE_QUTIM_VERSION_MAJOR}
#define QUTIM_VERSION_MINOR ${CMAKE_QUTIM_VERSION_MINOR}
#define QUTIM_VERSION_SECMINOR ${CMAKE_QUTIM_VERSION_SECMINOR}
#define QUTIM_VERSION_PATCH ${CMAKE_QUTIM_VERSION_PATCH}

#define QUTIM_MAKE_VERSION(a, b, c, d) (((a) << 24) | ((b) << 16) | (c) << 8 | (d))

#define QUTIM_VERSION \
	QUTIM_MAKE_VERSION(QUTIM_VERSION_MAJOR, QUTIM_VERSION_MINOR, QUTIM_VERSION_SECMINOR, QUTIM_VERSION_PATCH)

#define QUTIM_CHECK_VERSION(major, minor, secminor, patch) (QUTIM_VERSION >= QUTIM_MAKE_VERSION(major, minor, secminor, patch))

namespace qutim_sdk_0_3
{
LIBQUTIM_EXPORT quint8 versionMajor();
LIBQUTIM_EXPORT quint8 versionMinor();
LIBQUTIM_EXPORT quint8 versionSecMinor();
LIBQUTIM_EXPORT quint8 versionPatch();
LIBQUTIM_EXPORT quint32 version();
LIBQUTIM_EXPORT const char* versionString();
}

#endif
