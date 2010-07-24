#include "jmessagesession_p.h"
#include "jmessagehandler.h"
#include "../muc/jmucsession.h"
#include "jcontact.h"
#include "jabber_global.h"
#include <qutim/messagesession.h>
#include <gloox/message.h>
#include <gloox/receipt.h>
#include <qutim/debug.h>
#include "../muc/jsessionconvertor.h"

using namespace gloox;
using namespace qutim_sdk_0_3;

namespace Jabber
{
	inline qutim_sdk_0_3::ChatState gloox2qutIM(gloox::ChatStateType state)
	{
		switch (state) {
		case gloox::ChatStateActive:
			return qutim_sdk_0_3::ChatStateActive;
		case gloox::ChatStateInactive:
			return qutim_sdk_0_3::ChatStateInActive;
		case gloox::ChatStateGone:
			return qutim_sdk_0_3::ChatStateGone;
		case gloox::ChatStateComposing:
			return qutim_sdk_0_3::ChatStateComposing;
		case gloox::ChatStatePaused:
			return qutim_sdk_0_3::ChatStatePaused;
		default:
			return qutim_sdk_0_3::ChatStateInActive;
		}
	}

	inline gloox::ChatStateType qutIM2gloox(qutim_sdk_0_3::ChatState state)
	{
		switch (state) {
		case qutim_sdk_0_3::ChatStateActive:
			return gloox::ChatStateActive;
		case qutim_sdk_0_3::ChatStateInActive:
			return gloox::ChatStateInactive;
		case qutim_sdk_0_3::ChatStateGone:
			return gloox::ChatStateGone;
		case qutim_sdk_0_3::ChatStateComposing:
			return gloox::ChatStateComposing;
		case qutim_sdk_0_3::ChatStatePaused:
			return gloox::ChatStatePaused;
		default:
			return gloox::ChatStateInvalid;
		}
	}

	class JMessageReceiptFilter : public MessageFilter
	{
	public:
		JMessageReceiptFilter(JMessageSession *s, MessageSession *p) :
				MessageFilter(p), m_session(s), m_nextId(0) {}
		void decorate(gloox::Message &msg);
		void filter(gloox::Message &msg);
		inline void setNextId(quint64 id) { m_nextId = id; }
	protected:
		JMessageSession *m_session;
		quint64 m_nextId;
		QHash<QByteArray, quint64> m_messages;
	};

	void JMessageReceiptFilter::decorate(gloox::Message &msg)
	{
		if (m_nextId) {
			m_messages.insert(QByteArray(msg.id().data(), msg.id().size()), m_nextId);
			msg.addExtension(new Receipt(Receipt::Request));
			m_nextId = 0;
		}
	}

	void JMessageReceiptFilter::filter(gloox::Message &msg)
	{
		if (const Receipt *receipt = msg.findExtension<Receipt>(ExtReceipt)) {
			if (receipt->rcpt() == Receipt::Request) {
				gloox::Message message(msg.subtype(), msg.from());
#if 0
				// It's correct behaviour
				Client *client = static_cast<JAccount*>(m_session->account())->client();
				message.setID(client->getID());
#else
				// And it's legacy one, but compatible with qutIM 0.2 and other clients
				message.setID(msg.id());
#endif
				message.addExtension(new Receipt(Receipt::Received, msg.id()));
				send(message);
				return;
			}
			QHash<QByteArray, quint64>::iterator it = m_messages.find(QByteArray(receipt->id().data(), receipt->id().size()));
			if (it == m_messages.end())
				it = m_messages.find(QByteArray(msg.id().data(), msg.id().size()));
			if (it == m_messages.end())
				return;
			if (receipt->rcpt() == Receipt::Received) {
				QEvent *ev = new qutim_sdk_0_3::MessageReceiptEvent(it.value(), true);
				if (ChatSession *session = ChatLayer::get(m_session, false))
					qApp->postEvent(session, ev);
				m_messages.erase(it);
				return;
			}
		}
	}

	JMessageSession::JMessageSession(JMessageHandler *handler, ChatUnit *unit, gloox::MessageSession *session)
			: Buddy(handler->account()), d_ptr(new JMessageSessionPrivate)
	{
		Q_D(JMessageSession);
		d->account = handler->account();
		d->handler = handler;
		d->session = session;
		d->chatStateFilter = new ChatStateFilter(d->session);
		d->messageReceiptFilter = new JMessageReceiptFilter(this, d->session);
		d->chatStateFilter->registerChatStateHandler(this);
		d->session->registerMessageHandler(this);
		d->handler->prepareMessageSession(this);
		d->followChanges = session->threadID().empty();
		d->unit = unit;
		d->atDeathState = false;
		setMenuOwner(unit);
		connect(unit, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
	}

	JMessageSession::JMessageSession(ChatUnit *unit) :
			Buddy(unit->account()), d_ptr(new JMessageSessionPrivate)
	{
		Q_D(JMessageSession);
		d->account = static_cast<JAccount *>(unit->account());
		d->handler = d->account->messageHandler();
		d->session = new MessageSession(d->account->client(), JID(unit->id().toStdString()));
		d->chatStateFilter = new ChatStateFilter(d->session);
		d->chatStateFilter->registerChatStateHandler(this);
		d->session->registerMessageHandler(this);
		d->handler->prepareMessageSession(this);
		d->followChanges = true;
		d->unit = unit;
		d->handler->setSessionUnit(this, unit);
		setMenuOwner(unit);
		connect(unit, SIGNAL(titleChanged(QString)), this, SIGNAL(titleChanged(QString)));
		connect(unit, SIGNAL(destroyed()), this, SLOT(onUnitDeath()));
	}

	JMessageSession::~JMessageSession()
	{
		Q_D(JMessageSession);
		if (d->account) {
			d->account->client()->disposeMessageSession(d->session);
			d->account->messageHandler()->messageSessionKiled(this);
		}
	}
	
	void JMessageSession::convertToMuc()
	{
		Q_D(JMessageSession);
		if (d->atDeathState)
			return;
		new JSessionConvertor(this);
//		JMUCSession *session = * new JMUCSession(this);
//		ChatLayer::get(this, true)->setChatUnit(session);
		d->atDeathState = true;
	}

	QString JMessageSession::id() const
	{
		return QString::fromStdString(d_func()->session->threadID());
	}

	QString JMessageSession::title() const
	{
		Q_D(const JMessageSession);
		return d->unit ? d->unit->title() : id();
	}

	gloox::MessageSession *JMessageSession::session()
	{
		return d_func()->session;
	}

	bool JMessageSession::sendMessage(const qutim_sdk_0_3::Message &message)
	{
		Q_D(JMessageSession);

		if (d->atDeathState || account()->status() == Status::Offline)
			return false;
		
		d->messages << message;

		d->messageReceiptFilter->setNextId(message.id());
		d->session->send(message.text().toStdString(), message.property("subject").toString().toStdString());
		if (d->followChanges) {
			d->handler->setSessionId(this, id());
			d->followChanges = false;
		}
		return true;
	}

	void JMessageSession::handleMessage(const gloox::Message &msg, MessageSession *session)
	{
		Q_D(JMessageSession);
		Q_ASSERT((!session) || (d->session == session));
		if (d->followChanges) {
			d->handler->setSessionId(this, id());
			d->followChanges = false;
		}
		qutim_sdk_0_3::Message coreMsg(QString::fromStdString(msg.body()));
		coreMsg.setIncoming(true);
		if (const DelayedDelivery *when = msg.when())
			coreMsg.setTime(stamp2date(when->stamp()));
		else
			coreMsg.setTime(QDateTime::currentDateTime());
		if (!msg.subject().empty())
			coreMsg.setProperty("subject", QString::fromStdString(msg.subject()));
		d->messages << coreMsg;
		ChatLayer::get(this, true)->appendMessage(coreMsg);
	}

	bool JMessageSession::event(QEvent *ev)
	{
		if (ev->type() == ChatStateEvent::eventType()) {
			ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(ev);
			d_func()->chatStateFilter->setChatState(qutIM2gloox(chatEvent->chatState()));
			return true;
		}
		return QObject::event(ev);
	}
	
	void JMessageSession::onUnitDeath()
	{
		Q_D(JMessageSession);
		QString id = QString::fromStdString(d->session->target().bare());
		JContact *contact = qobject_cast<JContact*>(d->account->getUnit(id));
		if (contact) {
			d->session->resetResource();
			d->unit = contact;
		} else {
			deleteLater();
		}
	}

	void JMessageSession::handleChatState(const JID &from, gloox::ChatStateType state)
	{
		Q_UNUSED(from);
		setChatState(gloox2qutIM(state));
	}

	ChatUnit *JMessageSession::upperUnit()
	{
		Q_D(JMessageSession);
		return d->unit;
	}
}
