#include "xstatussender.h"
#include "xtraz.h"
#include "icqaccount.h"
#include "connection.h"
#include "xstatus.h"

namespace qutim_sdk_0_3 {

namespace oscar {

XStatusSenderList::XStatusSenderList()
{
}

XStatusSender *XStatusSenderList::getSender(IcqAccount *account)
{
	XStatusSender *r = m_senders.value(account);
	if (!r) {
		r = new XStatusSender(account);
		m_senders.insert(account, r);
	}
	return r;
}

void XStatusSenderList::accountDestroyed(QObject *obj)
{
	IcqAccount *account = reinterpret_cast<IcqAccount*>(obj);
	XStatusSender *r = m_senders.take(account);
	if (r)
		delete r;
}

void XStatusSender::sendXStatus(IcqContact *contact, quint64 cookie)
{
	static XStatusSenderList list;
	IcqAccount *account = contact->account();
	Status::Type status = account->status().type();
	if (status == Status::Offline || status == Status::Connecting) {
		// Cannot send any requests in that state
		return;
	}
	XStatusSender *r = list.getSender(contact->account());
	if (r->m_contacts.contains(contact)) {
		// This contact is already in queue
		contact->setProperty("lastXStatusRequestCookie", cookie);
		return;
	}
	if (r->m_contacts.isEmpty() &&
		QDateTime::currentDateTime().toTime_t() - r->m_lastTime >= TIMEOUT_BETWEEN_REQUESTS &&
		account->connection()->testRate(MessageFamily, MessageResponse, false))
	{
		Q_ASSERT(!r->m_timer.isActive());
		r->sendXStatusImpl(contact, cookie);
	} else {
		contact->setProperty("lastXStatusRequestCookie", cookie);
		r->m_contacts.push_back(contact);
		if (!r->m_timer.isActive())
			r->m_timer.start();
	}
}

void XStatusSender::sendXStatus()
{
	IcqContact *contact = m_contacts.first();
	bool removeFirst = false;
	if (!contact) {
		removeFirst = true;
	} else if (contact->account()->connection()->testRate(MessageFamily, MessageResponse, false)) {
		bool ok;
		quint64 cookie = contact->property("lastXStatusRequestCookie").toLongLong(&ok);
		if (ok)
			sendXStatusImpl(contact, cookie);
		else
			debug() << "lastXStatusRequestCookie property should hold a cookie";
		removeFirst = true;
	}
	if (removeFirst) {
		m_contacts.takeFirst();
		if (m_contacts.isEmpty())
			m_timer.stop();
	}
}

void XStatusSender::statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous)
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

XStatusSender::XStatusSender(IcqAccount *account) :
	m_lastTime(0)
{
	m_timer.setInterval(TIMEOUT_BETWEEN_REQUESTS * 1000);
	connect(&m_timer, SIGNAL(timeout()), SLOT(sendXStatus()));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void XStatusSender::sendXStatusImpl(IcqContact *contact, quint64 cookie)
{
	IcqAccount *account = contact->account();
	QVariantHash extStatus = account->property("xstatus").toHash();
	int index = xstatusIndexByName(extStatus.value("name").toString());
	XtrazResponse response("cAwaySrv", "OnRemoteNotification");
	response.setValue("CASXtraSetAwayMessage", "");
	response.setValue("uin", account->id());
	response.setValue("index", QString("%1").arg(index));
	response.setValue("title", extStatus.value("title").toString());
	response.setValue("desc", extStatus.value("description").toString());
	SNAC snac = response.snac(contact, cookie);
	account->connection()->send(snac, false);
	m_lastTime = QDateTime::currentDateTime().toTime_t();
}

} } // namespace qutim_sdk_0_3::oscar

