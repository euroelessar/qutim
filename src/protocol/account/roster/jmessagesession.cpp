#include "jmessagesession.h"
#include "jmessagehandler.h"
#include "../jaccount.h"
#include "jabber_global.h"
#include <qutim/message.h>
#include <gloox/message.h>
#include <gloox/chatstatefilter.h>

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

	struct JMessageSessionPrivate
	{
		JMessageHandler *handler;
		JAccount *account;
		MessageSession *session;
		ChatStateFilter *chatStateFilter;
		ChatUnit *unit;
		bool followChanges;
	};

	JMessageSession::JMessageSession(JMessageHandler *handler, ChatUnit *unit, gloox::MessageSession *session)
			: ChatUnit(handler->account()), d_ptr(new JMessageSessionPrivate)
	{
		Q_D(JMessageSession);
		d->account = handler->account();
		d->handler = handler;
		d->session = session;
		d->chatStateFilter = new ChatStateFilter(d->session);
		d->chatStateFilter->registerChatStateHandler(this);
		d->session->registerMessageHandler(this);
		d->followChanges = session->threadID().empty();
		d->unit = unit;
	}

	JMessageSession::JMessageSession(ChatUnit *unit) :
			ChatUnit(unit->account()), d_ptr(new JMessageSessionPrivate)
	{
		Q_D(JMessageSession);
		d->account = static_cast<JAccount *>(unit->account());
		d->handler = d->account->messageHandler();
		d->session = new MessageSession(d->account->client(), JID(unit->id().toStdString()));
		d->chatStateFilter = new ChatStateFilter(d->session);
		d->chatStateFilter->registerChatStateHandler(this);
		d->session->registerMessageHandler(this);
		d->followChanges = true;
		d->unit = unit;
		d->handler->setSessionUnit(this, unit);
	}

	JMessageSession::~JMessageSession()
	{
		Q_D(JMessageSession);
		d->account->client()->disposeMessageSession(d->session);
	}

	QString JMessageSession::id() const
	{
		return QString::fromStdString(d_func()->session->threadID());
	}

	QString JMessageSession::title() const
	{
		return d_func()->unit->title();
	}

	void JMessageSession::sendMessage(const qutim_sdk_0_3::Message &message)
	{
		Q_D(JMessageSession);
		d->session->send(message.text().toStdString(), message.property("subject").toString().toStdString());
		if (d->followChanges) {
			d->handler->setSessionId(this, id());
			d->followChanges = false;
		}
	}

	void JMessageSession::setChatState(qutim_sdk_0_3::ChatState state)
	{
		d_func()->chatStateFilter->setChatState(qutIM2gloox(state));
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
		if (!msg.subject().empty())
			coreMsg.setProperty("subject", QString::fromStdString(msg.subject()));
		ChatLayer::get(this)->appendMessage(coreMsg);
	}

	void JMessageSession::handleChatState(const JID &from, gloox::ChatStateType state)
	{
		Q_UNUSED(from);
		emit chatStateChanged(gloox2qutIM(state));
	}
}
