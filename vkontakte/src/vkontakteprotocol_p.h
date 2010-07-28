/****************************************************************************
 *  vkontakteprotocol_p.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef VKONTAKTEPROTOCOL_P_H
#define VKONTAKTEPROTOCOL_P_H
#include <QHash>
#include <QPointer>

class VAccount;
struct VkontakteProtocolPrivate
{
	inline VkontakteProtocolPrivate() :
		accounts_hash(new QHash<QString, QPointer<VAccount> > ())
	{ }
	inline ~VkontakteProtocolPrivate() { delete accounts_hash; }
	union
	{
		QHash<QString, QPointer<VAccount> > *accounts_hash;
		QHash<QString, VAccount *> *accounts;
	};
};

#endif // VKONTAKTEPROTOCOL_P_H
