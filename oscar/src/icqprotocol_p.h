/****************************************************************************
 *  icqprotocol_p.h
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

#ifndef ICQPROTOCOL_P_H
#define ICQPROTOCOL_P_H

#include "icqaccount.h"
#include "icqprotocol.h"

namespace Icq
{

struct IcqProtocolPrivate
{
	inline IcqProtocolPrivate() :
		accounts_hash(new QHash<QString, QPointer<IcqAccount> > ())
	{ }
	inline ~IcqProtocolPrivate() { delete accounts_hash; }
	void initActions();
	union
	{
		QHash<QString, QPointer<IcqAccount> > *accounts_hash;
		QHash<QString, IcqAccount *> *accounts;
	};
	QHash<IcqStatus, Status> statuses;
};

} // namespace Icq

#endif // ICQPROTOCOL_P_H
