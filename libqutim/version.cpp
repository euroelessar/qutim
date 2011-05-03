/****************************************************************************
 *  version.cpp
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

#include "libqutim_version.h"

namespace qutim_sdk_0_3
{
quint8 versionMajor()
{
	return QUTIM_VERSION_MAJOR;
}

quint8 versionMinor()
{
	return QUTIM_VERSION_MINOR;
}

quint8 versionSecMinor()
{
	return QUTIM_VERSION_SECMINOR;
}

quint8 versionPatch()
{
	return QUTIM_VERSION_PATCH;
}

quint32 version()
{
	return QUTIM_VERSION;
}

const char* versionString()
{
	return QUTIM_VERSION_STRING;
}
}
