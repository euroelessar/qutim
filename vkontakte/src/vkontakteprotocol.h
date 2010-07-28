/****************************************************************************
 *  vkontakteprotocol.h
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

#ifndef VKONTAKTEPROTOCOL_H
#define VKONTAKTEPROTOCOL_H
#include <qutim/protocol.h>
#include "vkontakte_global.h"
#include <qutim/debug.h>

struct VkontakteProtocolPrivate;
class LIBVKONTAKTE_EXPORT VkontakteProtocol : public Protocol
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VkontakteProtocol)
	Q_CLASSINFO("Protocol", "vkontakte")
public:
	VkontakteProtocol();
	virtual ~VkontakteProtocol();
	virtual Account* account(const QString& id) const;
	virtual void loadAccounts();
	virtual QList< Account* > accounts() const;
	static inline VkontakteProtocol *instance() { if (!self) warning() << "IcqProtocol isn't created"; return self; }
private:
	static VkontakteProtocol *self;
	QScopedPointer<VkontakteProtocolPrivate> d_ptr;
	friend class VAccountCreator;
};

#endif // VKONTAKTEPROTOCOL_H
