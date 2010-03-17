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
	html_support      = 0x0001,
	utf8_support      = 0x0002,
	srvrelay_support  = 0x0004
};

struct IcqContactPrivate
{
	Q_DECLARE_PUBLIC(IcqContact);
	void clearCapabilities();
	void requestNick();
	void setCapabilities(const Capabilities &caps);
	void setChatState(ChatState state);
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
	QList<FeedbagItem> items;
	QStringList tags;
	ChatState state;
	IcqContact *q_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IcqContactPrivate::CapabilityFlags)

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQCONTACT_PH_H
