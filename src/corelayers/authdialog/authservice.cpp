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
	if(event->type() == AuthorizationRequest::eventType()) {
		debug() << "New request";
		AuthorizationRequest *request = static_cast<AuthorizationRequest*>(event);
		AuthDialogPrivate *dialog = new AuthDialogPrivate();
		connect(dialog,SIGNAL(accepted()),SLOT(onAccepted()));
		connect(dialog,SIGNAL(rejected()),SLOT(onRejected()));
		dialog->show(request->contact(),
					 request->body(),
					 false);
		return true;
	} else if(event->type() == AuthorizationReply::eventType()) {
		handleReply(static_cast<AuthorizationReply*>(event));
		return true;
	}
	return QObject::event(event);
}

void AuthService::handleReply(AuthorizationReply *reply)
{
	switch(reply->replyType()) {
	case AuthorizationReply::New: {
		debug() << "New reply";
		AuthDialogPrivate *dialog = new AuthDialogPrivate();
		connect(dialog,SIGNAL(accepted()),SLOT(onAccepted()));
		connect(dialog,SIGNAL(rejected()),SLOT(onRejected()));
		dialog->show(reply->contact(),
					 reply->body(),
					 true);
		break;
	}
	case AuthorizationReply::Accepted:
	case AuthorizationReply::Rejected:
		Notifications::send(reply->body(),reply->contact()->name());
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
		AuthorizationReply event = AuthorizationReply(AuthorizationReply::Accept,c);
		qApp->sendEvent(c,&event);
	}
	else {
		debug() << "Send request";
		AuthorizationRequest event = AuthorizationRequest(c,dialog->text());
		qApp->sendEvent(c,&event);
	}
}

void AuthService::onRejected()
{
	AuthDialogPrivate *dialog = qobject_cast<AuthDialogPrivate*>(sender());
	Contact *c = dialog->contact();
	if(dialog->isIncoming()) {
		AuthorizationReply event = AuthorizationReply(AuthorizationReply::Reject,c);
		qApp->sendEvent(c,&event);
	} else
		c->deleteLater(); //remove temporary contact
}

} // namespace Core

