/****************************************************************************
 *  icqcontact_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "icqcontact.h"

#ifndef ICQCONTACT_PH_H
#define ICQCONTACT_PH_H

#include "capability.h"
#include "oscarconnection.h"
#include "feedbag.h"

namespace qutim_sdk_0_3 {

namespace oscar {

enum SsiBuddyTlvs
{
	SsiBuddyNick = 0x0131,
	SsiBuddyComment = 0x013c,
	SsiBuddyReqAuth = 0x0066,
	SsiBuddyTags = 0x023c
};

enum ContactCapabilityFlags
{
	rtf_support       = 0x0001,
	html_support      = 0x0002,
	typing_support    = 0x0004,
	aim_chat_support  = 0x0008,
	aim_image_support = 0x0010,
	xtraz_support     = 0x0020,
	utf8_support      = 0x0040,
	sendfile_support  = 0x0080,
	direct_support    = 0x0100,
	icon_support      = 0x0200,
	getfile_support   = 0x0400,
	srvrelay_support  = 0x0800,
	avatar_support    = 0x1000
};

struct IcqContactPrivate
{
	void clearCapabilities();
	FeedbagItem getNotInListGroup();
	Q_DECLARE_FLAGS(CapabilityFlags, ContactCapabilityFlags)
	IcqAccount *account;
	QString uin;
	QString name;
	Status status;
	QString avatar;
	quint16 version;
	CapabilityFlags flags;
	Capabilities capabilities;
	DirectConnectionInfo dc_info;
	QList<FeedbagItem> items;
	QStringList tags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IcqContactPrivate::CapabilityFlags)

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQCONTACT_PH_H
