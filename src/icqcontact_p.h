/****************************************************************************
 *  icqcontact_p.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

namespace Icq {

struct IcqContactPrivate
{
	IcqAccount *account;
	QString uin;
	QString name;
	quint16 user_id;
	quint16 group_id;
	Status status;
	quint16 version;

	bool rtf_support;
	bool html_support;
	bool typing_support;
	bool aim_chat_support;
	bool aim_image_support;
	bool xtraz_support;
	bool utf8_support;
	bool sendfile_support;
	bool direct_support;
	bool icon_support;
	bool getfile_support;
	bool srvrelay_support;
	bool avatar_support;
	Capabilities capabilities;
	Capabilities short_capabilities;
	DirectConnectionInfo dc_info;
};

} // namespace Icq

#endif // ICQCONTACT_PH_H
