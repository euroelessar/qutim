/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "astralsession.h"
#include "astralaccount.h"
#include <qutim/chatsession.h>
#include <qutim/message.h>

struct AstralSessionPrivate
{
	TextChannelPtr channel;
	AstralAccount *account;
};

AstralSession::AstralSession(const TextChannelPtr &channel, AstralAccount *acc) : ChatUnit(acc), p(new AstralSessionPrivate)
{
	p->channel = channel;
	p->account = acc;
//	TextChannel
}

AstralSession::~AstralSession()
{
}

QString AstralSession::id() const
{
	return p->channel->interface<Client::ChannelInterface>()->path();
}

bool AstralSession::sendMessage(const qutim_sdk_0_3::Message &message)
{
	if(!message.text().isEmpty())
	{
		PendingSendMessage *pend = p->channel->send(message.text());
		pend->setProperty("id", message.id());
		connect(pend, SIGNAL(finished(Tp::PendingOperation*)), this, SLOT(onMessageSent(Tp::PendingOperation*)));
		return true;
	}
	return false;
}

void AstralSession::onMessageReceived(const Tp::ReceivedMessage &message)
{
	if(ChatLayer *chat = ChatLayer::instance())
	{
		qutim_sdk_0_3::Message mes;
		AstralContact *contact = p->account->roster()->contact(message.sender());
		mes.setChatUnit(contact);
		mes.setText(message.text());
		mes.setTime(message.sent());
		chat->getSession(this, true)->appendMessage(mes);
	}
}

void AstralSession::onMessageSent(Tp::PendingOperation *operation)
{
	qDebug("Message number %d has %sbeen sent", operation->property("id").toInt(), operation->isError() ? "not " : "");
}

