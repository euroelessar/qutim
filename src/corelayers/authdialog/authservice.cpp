/****************************************************************************
 *  authservice.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "authservice.h"
#include "authdialogimpl_p.h"
#include <qutim/debug.h>
#include <qutim/authorizationdialog.h>
#include <qutim/notificationslayer.h>

namespace Core {

bool AuthService::event(QEvent *event)
{
	if(event->type() == Request::eventType()) {
		debug() << "New request";
		Request *request = static_cast<Request*>(event);
		AuthDialogPrivate *dialog = new AuthDialogPrivate();
		connect(dialog,SIGNAL(accepted()),SLOT(onAccepted()));
		connect(dialog,SIGNAL(rejected()),SLOT(onRejected()));
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
		connect(dialog,SIGNAL(accepted()),SLOT(onAccepted()));
		connect(dialog,SIGNAL(rejected()),SLOT(onRejected()));
		dialog->show(reply->contact(),
					 reply->body(),
					 true);
		break;
	}
	case Reply::Accepted:
	case Reply::Rejected:
		Notifications::send(Notifications::System,reply->contact(),reply->body());
		break;
	default:
		break;
	}
}

void AuthService::onAccepted()
{
	AuthDialogPrivate *dialog = qobject_cast<AuthDialogPrivate*>(sender());
	Contact *c = dialog->contact();
	if(dialog->isIncoming()) {
		debug() << "Send reply";
		Reply event = Reply(Reply::Accept,c);
		qApp->sendEvent(c,&event);
	}
	else {
		debug() << "Send request";
		Request event = Request(c,dialog->text());
		qApp->sendEvent(c,&event);
	}
}

void AuthService::onRejected()
{
	AuthDialogPrivate *dialog = qobject_cast<AuthDialogPrivate*>(sender());
	Contact *c = dialog->contact();
	if(dialog->isIncoming()) {
		Reply event = Reply(Reply::Reject,c);
		qApp->sendEvent(c,&event);
	} else
		c->deleteLater(); //remove temporary contact
}

} // namespace Core

