/****************************************************************************
 *  quetzaleventloop.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef QUETZALEVENTLOOP_H
#define QUETZALEVENTLOOP_H

#include <QSocketNotifier>
#include <purple.h>
#include <QMap>
#include <QMutex>

class QAction;

class QuetzalEventLoop : public QObject
{
	Q_OBJECT
	struct TimerInfo
	{
		TimerInfo(GSourceFunc f, gpointer d) : function(f), data(d) {}
		GSourceFunc function;
		gpointer data;
	};
	struct FileInfo
	{
		FileInfo(int fd_, QSocketNotifier *s, PurpleInputCondition c, PurpleInputFunction f, gpointer d) :
				fd(fd_), socket(s), cond(c), func(f), data(d) {}
		int fd;
		QSocketNotifier *socket;
		PurpleInputCondition cond;
		PurpleInputFunction func;
		gpointer data;
	};

public:
	static QuetzalEventLoop *instance();
	uint addTimer(guint interval, GSourceFunc function, gpointer data);
	gboolean removeTimer(guint handle);
	guint addIO(int fd, PurpleInputCondition cond, PurpleInputFunction func, gpointer user_data);
	gboolean removeIO(guint handle);
	int getIOError(int fd, int *error);
	Q_INVOKABLE void startTimer(int interval, int *id);
public slots:
	void onAction(QAction *action);
protected:
	virtual void timerEvent(QTimerEvent *event);
private slots:
	void onSocket(int fd);

private:
	explicit QuetzalEventLoop(QObject *parent = 0);
	static QuetzalEventLoop *m_self;
	QMutex m_timerMutex;
	QMap<int, TimerInfo *> m_timers;
	QMap<guint, FileInfo *> m_files;
	guint m_socketId;
};

extern PurpleEventLoopUiOps quetzal_eventloop_uiops;

#endif // QUETZALEVENTLOOP_H
