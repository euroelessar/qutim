/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef METACONTACTPROTOCOL_P_H
#define METACONTACTPROTOCOL_P_H

#include "protocol_p.h"

namespace qutim_sdk_0_3
{
	class MetaContactManager;
	
	class MetaProtocolPrivate : public ProtocolPrivate
	{
	public:
		MetaContactManager *manager;
	};
	
	class MetaProtocol : public Protocol
	{
		Q_OBJECT
		Q_CLASSINFO("Protocol", "meta")
		Q_DECLARE_PRIVATE(MetaProtocol)
	public:
		MetaProtocol(MetaContactManager *manager);
		~MetaProtocol();
		virtual QList<Account *> accounts() const;
		virtual Account *account(const QString &id) const;
	private:
		virtual void loadAccounts();
	};
}

#endif // METACONTACTPROTOCOL_P_H
