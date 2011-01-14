/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef JSONHISTORY_H
#define JSONHISTORY_H

#include <qutim/history.h>
#include <QDir>
#include <QPointer>

using namespace qutim_sdk_0_3;

namespace Core
{
	class HistoryWindow;
	class JsonHistory : public History
	{
		Q_OBJECT
	public:
		JsonHistory();
		virtual ~JsonHistory();
		uint findEnd(QFile &file);
		virtual void store(const Message &message);
		virtual MessageList read(const ChatUnit *unit, const QDateTime &from, const QDateTime &to, int max_num);
		virtual void showHistory(const ChatUnit *unit);
		static QString quote(const QString &str);
		static QString unquote(const QString &str);
	private slots:
		void onHistoryActionTriggered(QObject *object);
	private:
		QString getFileName(const Message &message) const;
		QDir getAccountDir(const ChatUnit *unit) const;
		struct EndValue
		{
			QDateTime lastModified;
			uint end;
		};
		QPointer<HistoryWindow> m_history_window;
		typedef QHash<QString, EndValue> EndCache;
		EndCache m_cache;
	};
}

#endif // JSONHISTORY_H
