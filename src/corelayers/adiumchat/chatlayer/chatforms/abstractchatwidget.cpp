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

namespace Core
{
namespace AdiumChat
{

AbstractChatWidget::AbstractChatWidget(QWidget *parent) :
	QMainWindow(parent)
{

}

void AbstractChatWidget::addSessions(const ChatSessionList &sessions)
{
	foreach(ChatSessionImpl *s,sessions)
		addSession(s);
}

}
}
