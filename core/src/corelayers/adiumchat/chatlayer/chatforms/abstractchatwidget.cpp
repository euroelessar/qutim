/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "abstractchatwidget.h"
#include <qutim/icon.h>
#include <qutim/conference.h>
#include <QAbstractItemModel>
#include <3rdparty/avatarfilter/avatarfilter.h>

namespace Core
{
namespace AdiumChat
{

AbstractChatWidget::AbstractChatWidget(QWidget *parent) :
	QMainWindow(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
}

void AbstractChatWidget::addSessions(const ChatSessionList &sessions)
{
	foreach(ChatSessionImpl *s,sessions)
		addSession(s);
}

void AbstractChatWidget::addActions(const QList<ActionGenerator *> &actions)
{
	foreach(ActionGenerator *gen,actions)
		addAction(gen);
}

void AbstractChatWidget::setTitle(ChatSessionImpl *s)
{
	ChatUnit *u = s->getUnit();
	QIcon icon = Icon("view-choose");
	QString title;
	if(s->unread().count())
		title = tr("Chat with %1 (have %2 unread messages)").arg(u->title()).arg(s->unread().count());
	else
		title = tr("Chat with %1").arg(u->title());
	if (Conference *c = qobject_cast<Conference *>(u)) {
		icon = Icon("meeting-attending"); //TODO
		title = tr("Conference %1 (%2)").arg(c->title(),c->id());
	} else {
		if (Buddy *b = qobject_cast<Buddy*>(u)) {
			if (b->avatar().isEmpty())
				icon = Icon("view-choose");
			else {
				QIcon overlay = b->status().icon();
				icon = AvatarFilter::icon(b->avatar(), overlay);
			}
		}
	}

	window()->setWindowFilePath(title);
	window()->setWindowIcon(icon);
}

QString AbstractChatWidget::titleForSession(ChatSessionImpl *s)
{
	ChatUnit *u = s->getUnit();
	QString title;
	if(s->unread().count())
		title = tr("Chat with %1 (have %2 unread messages)").arg(u->title()).arg(s->unread().count());
	else
		title = tr("Chat with %1").arg(u->title());
	bool isContactsViewVisible;
	if (Conference *c = qobject_cast<Conference *>(u)) {
		title = tr("Conference %1 (%2)").arg(c->title(),c->id());
		isContactsViewVisible = true;
	} else {
		isContactsViewVisible = s->getModel()->rowCount(QModelIndex()) > 0;
	}
	return title;
}


}
}
