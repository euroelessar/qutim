/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#include "quickauthdialog.h"
#include "authdialogwrapper.h"
#include <qdeclarative.h>
#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>
#include <qutim/debug.h>
#include <QApplication>

namespace MeegoIntegration
{

QuickAuthDialog::QuickAuthDialog() {
}

void QuickAuthDialog::accept(const QString &message, bool isIncoming, qutim_sdk_0_3::Contact* c)
{
	if(isIncoming) {
		qDebug() << "Send reply";
		Reply event = Reply(Reply::Accept, c);
		qApp->sendEvent(c,&event);
	}
	else {
		qDebug() << "Send request";
		Request event = Request(c,message);
		qApp->sendEvent(c,&event);
	}
}

void QuickAuthDialog::cancel(Contact* c, bool isIncoming)
{
	if(isIncoming) {
		Reply event = Reply(Reply::Reject, c);
		qApp->sendEvent(c, &event);
	}
}

bool QuickAuthDialog::event(QEvent *event)
{
	if(event->type() == Request::eventType()) {
		qDebug() << "New request";
		Request *request = static_cast<Request*>(event);
		AuthDialogWrapper::showDialog(request->contact(),request->body(),false,this);
		return true;
	} else if(event->type() == Reply::eventType()) {
		handleReply(static_cast<Reply*>(event));
		return true;
	}
	return QObject::event(event);
}

void QuickAuthDialog::handleReply(Reply *reply)
{
	qDebug() << "New reply"  << reply->replyType();
	switch(reply->replyType()) {
	case Reply::New: {
		AuthDialogWrapper::showDialog(reply->contact(),reply->body(),true,this);
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
}
