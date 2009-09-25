/****************************************************************************
 *  protocol.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "protocol.h"
#include "account.h"
#include "contact.h"

namespace qutim_sdk_0_3
{
	struct ProtocolPrivate
	{
		QString id;
	};

	Protocol::Protocol() : p(new ProtocolPrivate)
	{
	}

	Protocol::~Protocol()
	{
	}

	Config Protocol::config()
	{
		return Config(id());
	}

	ConfigGroup Protocol::config(const QString &group)
	{
		return config().group(group);
	}

	QString Protocol::id()
	{
		if(p->id.isNull())
			p->id = QString::fromUtf8(metaInfo(metaObject(), "Protocol"));
		return p->id;
	}
}
