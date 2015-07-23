/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2015 Nicolay Izoderov <nico-izo@ya.ru>
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

#ifndef SQLITEHISTORY_H
#define SQLITEHISTORY_H

#include <qutim/history.h>
#include <QRunnable>
#include <QDir>
#include <QLinkedList>
#include <QPointer>
#include <QMutex>
#include <QQueue>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

using namespace qutim_sdk_0_3;

namespace Core
{
class SqliteWorker;

class SqliteHistory : public History
{
	Q_OBJECT
public:
	SqliteHistory();
	virtual ~SqliteHistory();

	void store(const Message &message) override;
	AsyncResult<MessageList> read(const ContactInfo &info, const QDateTime &from, const QDateTime &to, int max_num) override;
	AsyncResult<QVector<AccountInfo>> accounts() override;
	AsyncResult<QVector<ContactInfo>> contacts(const AccountInfo &account) override;
	AsyncResult<QList<QDate>> months(const ContactInfo &contact, const QString &needle) override;
	AsyncResult<QList<QDate>> dates(const ContactInfo &contact, const QDate &month, const QString &needle) override;

public slots:
	void errorHandler(const QString & error);

private:
	QThread* m_thread;
	SqliteWorker* m_worker;
};

class SqliteWorker : public QObject
{
	Q_OBJECT
public:

	enum MessageType {
		Message = 0,
		Topic = 1,
		Service = 2
	};
	Q_DECLARE_FLAGS(MessageTypes, MessageType)

	static QString escapeSqliteLike(const QString &str);
	void runJob(std::function<void ()> job);
	void shutdown();
public slots:
	void process();
	//void deleteLater();
signals:
	void finished();
	void error(const QString &error);

private slots:
	void exec();
private:
	inline int currentVersion() { return 1; }
	void makeMigration(int version);
	QQueue<std::function<void ()>> m_queue;
	QMutex m_queueLock;
	QMutex m_runningLock;
	QSqlDatabase m_db;
	void prepareDb();
	bool m_isRunning = false;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SqliteWorker::MessageTypes)

}

#endif // SQLITEHISTORY_H

