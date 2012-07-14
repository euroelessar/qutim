/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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
#include "jattention.h"
#include <qutim/icon.h>
#include <qutim/event.h>
#include <qutim/notification.h>
#include "../../account/jaccount.h"
#include "../../account/roster/jcontact.h"
#include "../../account/roster/jcontactresource.h"
#include <jreen/message.h>
#include <jreen/attention.h>
#include <qutim/chatunit.h>
#include "../../account/roster/jmessagehandler.h"

namespace Jabber {

using namespace qutim_sdk_0_3;

Q_GLOBAL_STATIC(JAttentionInfo::WeakPtr, weakInfo)

JAttention::JAttention()
{
	JAttentionInfo::WeakPtr &info = *weakInfo();
	if (info) {
		m_info = info.toStrongRef();
	} else {
		m_info = JAttentionInfo::Ptr::create();
		m_info->generator.reset(new SendAttentionActionGenerator(
									this, SLOT(onAttentionClicked(QObject*))));
		MenuController::addAction<JContact>(m_info->generator.data());
		info = m_info.toWeakRef();
	}
}

JAttention::~JAttention()
{
}

void JAttention::init(qutim_sdk_0_3::Account *account)
{
	m_account = account;
	JAccount *acc = qobject_cast<JAccount*>(account);
	Jreen::Client *client = qobject_cast<Jreen::Client*>(acc->property("client").value<QObject*>());
	connect(client, SIGNAL(messageReceived(Jreen::Message)), this, SLOT(onMessageReceived(Jreen::Message)));
	// ChatSession! Handle activated() !
}

SendAttentionActionGenerator::SendAttentionActionGenerator(QObject *obj, const char *slot)
	: ActionGenerator(QIcon(), LocalizedString(), obj, slot)
{
	setType(ActionTypeChatButton | ActionTypeContactList);
}

void SendAttentionActionGenerator::createImpl(QAction *action, QObject *obj) const
{
	Q_UNUSED(obj);
	Q_UNUSED(action);
}

void SendAttentionActionGenerator::showImpl(QAction *action, QObject *obj)
{
	JContact *contact = qobject_cast<JContact*>(obj);
	Q_ASSERT(contact);

	action->setEnabled(true);

	action->setText(QObject::tr("Send Attention"));
	action->setIcon(Icon(QLatin1String("dialog-warning")));
}

void JAttention::onAttentionClicked(QObject *obj)
{
	JContact *contact = qobject_cast<JContact*>(obj);
	Q_ASSERT(contact);

	QString resource;
	for(int i = 0; i < contact->resources().count(); ++i) {
		if(contact->resources().at(i)->checkFeature(QLatin1String("urn:xmpp:attention:0")))
			resource = contact->resources().at(i)->name();
	}

	if(resource.isEmpty()) {
		onAttentionDone(false);
		return;
	}

	Jreen::Client *client = qobject_cast<Jreen::Client*>(contact->account()->property("client").value<QObject*>());
	Jreen::JID jid(contact->id());
	jid.setResource(resource);
	Jreen::Message msg(Jreen::Message::Headline, jid);
	msg.addPayload(new Jreen::Attention);
	client->send(msg);
	// Send attention
	onAttentionDone(true);
}

void JAttention::onMessageReceived(const Jreen::Message &message)
{
	if(message.containsPayload<Jreen::Attention>() && message.body().isEmpty()) {
		NotificationRequest request(Notification::Attention);
		//request.setObject(this);
		request.setText(tr("%1 trying to attract your attention!").arg(message.from()));
		request.send();
		debug() << "ATTENTION!!!";
	}
}

void JAttention::onAttentionDone(bool success)
{
	NotificationRequest request(Notification::System);
	if(!success)
		request.setText(tr("Contact's client does not suppoort attentions"));
	else
		request.setText(tr("Trying to attract attention."));
	request.send();
}

} // namespace Jabber

