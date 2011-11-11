/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
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

