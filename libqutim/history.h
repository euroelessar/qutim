/****************************************************************************
 *  history.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef HISTORY_H
#define HISTORY_H

#include "message.h"
#include <QDateTime>

namespace qutim_sdk_0_3
{
	class ChatUnit;

	class LIBQUTIM_EXPORT History : public QObject
	{
		Q_OBJECT
	public:
		History();
		virtual ~History();
		static History *instance();
		virtual void store(const Message &message);
		virtual MessageList read(const ChatUnit *unit, const QDateTime &from, const QDateTime &to, int max_num);
		MessageList read(const ChatUnit *unit, const QDateTime &to, int max_num) { return read(unit, QDateTime(), to, max_num); }
		MessageList read(const ChatUnit *unit, int max_num) { return read(unit, QDateTime(), QDateTime::currentDateTime(), max_num); }
	public slots:
		virtual void showHistory(const ChatUnit *unit);
		virtual void virtual_hook(int id, void *data);
	};
}

#endif // HISTORY_H
