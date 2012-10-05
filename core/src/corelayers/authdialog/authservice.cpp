/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "authservice.h"
#include "authdialogimpl_p.h"
#include <qutim/debug.h>
#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>

namespace Core {

bool AuthService::event(QEvent *event)
{
	if(event->type() == Request::eventType()) {
		debug() << "New request";
		Request *request = static_cast<Request*>(event);
		AuthDialogPrivate *dialog = new AuthDialogPrivate();
		connect(dialog,SIGNAL(accepted()), SLOT(onAccepted()));
		connect(dialog,SIGNAL(rejected()), SLOT(onRejected()));
		dialog->show(request->contact(),
					 request->body(),
					 false);
		return true;
	} else if(event->type() == Reply::eventType()) {		
		handleReply(static_cast<Reply*>(event));
		return true;
	}
	return QObject::event(event);
}

void AuthService::handleReply(Reply *reply)
{
	debug() << "New reply"  << reply->replyType();
	switch(reply->replyType()) {
	case Reply::New: {
		AuthDialogPrivate *dialog = new AuthDialogPrivate();
		connect(dialog,SIGNAL(accepted()), SLOT(onAccepted()));
		connect(dialog,SIGNAL(rejected()), SLOT(onRejected()));
		dialog->show(reply->contact(),
					 reply->body(),
					 true);
		break;
	}
	case Reply::Accepted:
	case Reply::Rejected: {
		NotificationRequest request(Notification::System);
		request.setObject(reply->contact());
		request.setText(reply->body());
		request.send();
		break;
	}
	default:
		break;
	}
}

void AuthService::onAccepted()
{
	AuthDialogPrivate *dialog = qobject_cast<AuthDialogPrivate*>(sender());
	if (Contact *contact = dialog->contact()) {
		if (dialog->isIncoming()) {
			debug() << "Send reply";
			Reply event = Reply(Reply::Accept, contact);
			qApp->sendEvent(contact,&event);
		} else {
			debug() << "Send request";
			Request event = Request(contact,dialog->text());
			qApp->sendEvent(contact,&event);
		}
	}
}

void AuthService::onRejected()
{
	AuthDialogPrivate *dialog = qobject_cast<AuthDialogPrivate*>(sender());
	if (Contact *contact = dialog->contact()) {
		if (dialog->isIncoming()) {
			Reply event = Reply(Reply::Reject, contact);
			qApp->sendEvent(contact, &event);
		} else {
			contact->deleteLater(); //remove temporary contact
		}
	}
}

} // namespace Core

