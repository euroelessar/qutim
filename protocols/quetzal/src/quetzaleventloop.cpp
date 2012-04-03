/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "quetzaleventloop.h"
#include <qutim/debug.h>
#include <QTimerEvent>
#include <QThread>
#include <QCoreApplication>
#include <QVariant>
#include <QTimer>

Q_DECLARE_METATYPE(int*)

using namespace qutim_sdk_0_3;

QuetzalEventLoop *QuetzalEventLoop::m_self = NULL;

QuetzalEventLoop::QuetzalEventLoop(QObject *parent):
		QObject(parent), m_socketId(0)
{
	qRegisterMetaType<int*>("int*");
}

QuetzalEventLoop *QuetzalEventLoop::instance()
{
	if (!m_self) {
		m_self = new QuetzalEventLoop();
	}
	return m_self;
}

//static GSourceFunc quetzal_accounts_save_cb = NULL;

uint QuetzalEventLoop::addTimer(guint interval, GSourceFunc function, gpointer data)
{
//	// This hook is used for plugin to know when accounts's data should be saved
//	if (!quetzal_accounts_save_cb && interval == 5000)
//		quetzal_accounts_save_cb = function;
	bool isMainThread = QThread::currentThread() == qApp->thread();
	int id = -1;
	if (isMainThread) {
		id = QObject::startTimer(interval);
	} else {
		QMetaObject::invokeMethod(this, "startTimer", Qt::BlockingQueuedConnection,
								  Q_ARG(int, interval), Q_ARG(int*, &id));
	}
	QMutexLocker locker(&m_timerMutex);
	m_timers.insert(id, new TimerInfo(function, data));
	return static_cast<uint>(id);
}

void QuetzalEventLoop::startTimer(int interval, int *id)
{
	QMutexLocker locker(&m_timerMutex);
	*id = QObject::startTimer(interval);
}

gboolean QuetzalEventLoop::removeTimer(guint handle)
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());
	int id = static_cast<int>(handle);
	QMutexLocker locker(&m_timerMutex);
	QMap<int, TimerInfo *>::iterator it = m_timers.find(id);
	if (it == m_timers.end())
		return FALSE;
	killTimer(id);
	delete it.value();
	m_timers.erase(it);
	return TRUE;
}

void QuetzalEventLoop::timerEvent(QTimerEvent *event)
{
	m_timerMutex.lock();
	QMap<int, TimerInfo *>::iterator it = m_timers.find(event->timerId());
	if (it == m_timers.end()) {
		m_timerMutex.unlock();
		return;
	}
	TimerInfo info = *it.value();
	m_timerMutex.unlock();
//	if (info.function == quetzal_accounts_save_cb)
//		purple_blist_schedule_save();
	gboolean result = (*info.function)(info.data);
	if (!result) {
		QMutexLocker locker(&m_timerMutex);
		it = m_timers.find(event->timerId());
		if (it == m_timers.end())
			return;
		killTimer(it.key());
		delete it.value();
		m_timers.erase(it);
	}
}

guint QuetzalEventLoop::addIO(int fd, PurpleInputCondition cond, PurpleInputFunction func, gpointer user_data)
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());
	if (fd < 0) {
		warning() << "Invalid file descriptor" << fd << "return id" << m_socketId;
		return m_socketId++;
	}

	QSocketNotifier::Type type;
	if (cond & PURPLE_INPUT_READ)
		type = QSocketNotifier::Read;
	else
		type = QSocketNotifier::Write;

	QSocketNotifier *socket = new QSocketNotifier(fd, type, this);
	socket->setProperty("quetzal_id", m_socketId);
	connect(socket, SIGNAL(activated(int)), this, SLOT(onSocket(int)));

	m_files.insert(m_socketId, new FileInfo(fd, socket, cond, func, user_data));
	socket->setEnabled(true);
	return m_socketId++;
}

gboolean QuetzalEventLoop::removeIO(guint handle)
{
	Q_ASSERT(QThread::currentThread() == qApp->thread());
	QMap<uint, FileInfo *>::iterator it = m_files.find(handle);
	if (it == m_files.end())
		return FALSE;
	FileInfo *info = it.value();
	info->socket->deleteLater();
	m_files.erase(it);
	delete info;
	return TRUE;
}

int QuetzalEventLoop::getIOError(int fd, int *error)
{
	Q_UNUSED(fd);
	Q_UNUSED(error);
	return 0;
}

void QuetzalEventLoop::onSocket(int fd)
{
	QSocketNotifier *socket = qobject_cast<QSocketNotifier *>(sender());
	guint id = socket->property("quetzal_id").toUInt();
	QMap<uint, FileInfo *>::iterator it = m_files.find(id);
	if (it != m_files.end()) {
		FileInfo *info = it.value();
		socket->setEnabled(false);
		(*info->func)(info->data, fd, info->cond);
		socket->setEnabled(true);
	}
}

static guint quetzal_timeout_add(guint interval, GSourceFunc function, gpointer data)
{
	return QuetzalEventLoop::instance()->addTimer(interval, function, data);
}

static gboolean quetzal_timeout_remove(guint handle)
{
	return QuetzalEventLoop::instance()->removeTimer(handle);
}

static guint quetzal_input_add(int fd, PurpleInputCondition cond, PurpleInputFunction func, gpointer user_data)
{
	return QuetzalEventLoop::instance()->addIO(fd, cond, func, user_data);
}

static gboolean quetzal_input_remove(guint handle)
{
	return QuetzalEventLoop::instance()->removeIO(handle);
}

//static int quetzal_input_get_error(int fd, int *error)
//{
//	return QuetzalTimer::instance()->getIOError(fd, error);
//}

static guint quetzal_timeout_add_seconds(guint interval, GSourceFunc function, gpointer data)
{
	return quetzal_timeout_add(interval * 1000, function, data);
}

PurpleEventLoopUiOps quetzal_eventloop_uiops =
{
	quetzal_timeout_add,
	quetzal_timeout_remove,
	quetzal_input_add,
	quetzal_input_remove,
	NULL /*quetzal_input_get_error*/,
	quetzal_timeout_add_seconds,

	/* padding */
	NULL,
	NULL,
	NULL
};

