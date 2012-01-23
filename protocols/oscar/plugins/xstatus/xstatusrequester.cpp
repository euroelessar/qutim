/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "xstatusrequester.h"
#include "../../src/xtraz.h"
#include "../../src/icqaccount.h"
#include "../../src/connection.h"
#include "xstatus.h"

namespace qutim_sdk_0_3 {

namespace oscar {

XStatusRequesterList::XStatusRequesterList()
{
}

XStatusRequester *XStatusRequesterList::getRequester(IcqAccount *account)
{
	XStatusRequester *r = m_requesters.value(account);
	if (!r) {
		r = new XStatusRequester(account);
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
	Status::Type status = account->status().type();
	if (status == Status::Offline || status == Status::Connecting) {
		// Cannot send any requests in that state
		return;
	}
	XStatusRequester *r = list.getRequester(contact->account());
	if (r->m_contacts.contains(contact)) {
		// This contact is already in queue
		return;
	}
	if (r->m_contacts.isEmpty() &&
		QDateTime::currentDateTime().toTime_t() - r->m_lastTime >= TIMEOUT_BETWEEN_REQUESTS &&
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
	IcqContact *contact = m_contacts.first().data();
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


void XStatusRequester::statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous)
{
	bool wasOffline = previous == Status::Offline || previous == Status::Connecting;
	bool isOffline = current == Status::Offline || current == Status::Connecting;
	if (!wasOffline && isOffline) {
		// Account changed status to offline,
		// thus we need to clear requests queue
		m_contacts.clear();
		m_timer.stop();
	}
}

XStatusRequester::XStatusRequester(IcqAccount *account) :
	m_lastTime(0)
{
	m_timer.setInterval(TIMEOUT_BETWEEN_REQUESTS * 1000);
	connect(&m_timer, SIGNAL(timeout()), SLOT(updateXStatus()));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void XStatusRequester::updateXStatusImpl(IcqContact *contact)
{
	IcqAccount *account = contact->account();
	XtrazRequest request("cAwaySrv", "srvMng");
	request.setValue("id", "AwayStat");
	request.setValue("trans", "1");
	request.setValue("senderId", account->id());
	SNAC snac = request.snac(contact);
	account->connection()->send(snac, false);
	m_lastTime = QDateTime::currentDateTime().toTime_t();
}

} } // namespace qutim_sdk_0_3::oscar

