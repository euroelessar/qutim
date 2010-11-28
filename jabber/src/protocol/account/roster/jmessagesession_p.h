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

#ifndef JMESSAGESESSION_P_H
#define JMESSAGESESSION_P_H

#include "../jaccount.h"
#include <qutim/message.h>
#include <gloox/chatstatefilter.h>

namespace Jabber
{
class JMessageReceiptFilter;

class JMessageSessionPrivate
{
public:
	QPointer<JMessageHandler> handler;
	QPointer<JAccount> account;
	MessageSession *session;
	ChatStateFilter *chatStateFilter;
	JMessageReceiptFilter *messageReceiptFilter;
	QPointer<ChatUnit> unit;
	bool followChanges;
	MessageList messages;
	bool atDeathState;
	QString id;
};
}
#endif // JMESSAGESESSION_P_H
