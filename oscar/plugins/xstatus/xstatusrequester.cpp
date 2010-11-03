/****************************************************************************
 *  xstatusrequester.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "xstatusrequester.h"
#include "xtraz.h"
#include "icqaccount.h"
#include "connection.h"

namespace qutim_sdk_0_3 {

namespace oscar {

XStatusRequesterList::XStatusRequesterList()
{
}

XStatusRequester *XStatusRequesterList::getRequester(IcqAccount *account)
{
	XStatusRequester *r = m_requesters.value(account);
	if (!r) {
		r = new XStatusRequester();
		m_requesters.insert(account, r);
	}
	return r;
}

void XStatusRequesterList::accountDestroyed(QObject *obj)
{
	IcqAccount *account = reinterpret_cast<IcqAccount*>(obj);
	XStatusRequester *r = m_requesters.take(account);
	if (r)
		delete r;
}

void XStatusRequester::updateXStatus(IcqContact *contact)
{
	static XStatusRequesterList list;
	IcqAccount *account = contact->account();
	XStatusRequester *r = list.getRequester(contact->account());
	if (r->m_contacts.contains(contact)) {
		// This contact is already in queue
		return;
	}
	if (r->m_contacts.isEmpty() &&
		account->connection()->testRate(MessageFamily, MessageSrvSend, false))
	{
		Q_ASSERT(!r->m_timer.isActive());
		r->updateXStatusImpl(contact);
	} else {
		r->m_contacts.push_back(contact);
		if (!r->m_timer.isActive())
			r->m_timer.start();
	}
}

void XStatusRequester::updateXStatus()
{
	IcqContact *contact = m_contacts.first();
	bool removeFirst = false;
	if (!contact) {
		removeFirst = true;
	} else if (contact->account()->connection()->testRate(MessageFamily, MessageSrvSend, false)) {
		updateXStatusImpl(contact);
		removeFirst = true;
	}
	if (removeFirst) {
		m_contacts.takeFirst();
		if (m_contacts.isEmpty())
			m_timer.stop();
	}
}

XStatusRequester::XStatusRequester()
{
	m_timer.setInterval(5000);
	connect(&m_timer, SIGNAL(timeout()), SLOT(updateXStatus()));
}

void XStatusRequester::updateXStatusImpl(IcqContact *contact)
{
	IcqAccount *account = contact->account();
	XtrazRequest request("cAwaySrv", "srvMng");
	request.setValue("id", "AwayStat");
	request.setValue("trans", "1");
	request.setValue("senderId", account->id());
	SNAC snac = request.snac(contact);
	account->connection()->send(snac, 10);
}

} } // namespace qutim_sdk_0_3::oscar
