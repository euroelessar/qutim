/****************************************************************************
 *  roster.h
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

#ifndef ROSTER_H
#define ROSTER_H

#include "tlv.h"

namespace Icq
{
	class IcqContact;
	class IcqAccount;
}

namespace qutim_sdk_0_3
{

using namespace Icq;
class Account;

class LIBOSCAR_EXPORT RosterPlugin
{
public:
	RosterPlugin() {}
	virtual ~RosterPlugin() { }
	virtual void statusChanged(IcqContact *contact, const Status &status, const TLVMap &tlvs) = 0;
	virtual void virtual_hook(int type, void *data) = 0;
	inline void setAccount(Account *account) { m_account = account; }
protected:
	union
	{
		IcqAccount *m_icq_account;
		Account *m_account;
	};
};

} // namespace qutim_sdk_0_3

Q_DECLARE_INTERFACE(qutim_sdk_0_3::RosterPlugin, "org.qutim.oscar.RosterPlugin");

#endif // ROSTER_H
