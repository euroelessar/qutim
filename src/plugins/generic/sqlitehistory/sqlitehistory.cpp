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

#include "sqlitehistory.h"
#include <qutim/chatunit.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/systeminfo.h>
#include <QStringBuilder>
#include <QThreadPool>
#include <qutim/icon.h>
#include <qutim/debug.h>
//#include <QElapsedTimer>
#include <QQueue>
#include <QtSql/QSqlError>
#include <QRegularExpression>
#include <QTimer>

namespace Core
{

SqliteHistory::SqliteHistory() :
	m_thread(new QThread),
	m_worker(new SqliteWorker)
{

	m_worker->moveToThread(m_thread);

	connect(m_worker, SIGNAL(error(QString)), this, SLOT(errorHandler(QString)));

	connect(m_thread, SIGNAL(started()), m_worker, SLOT(process()));

	connect(m_worker, SIGNAL(finished()), m_thread, SLOT(quit()));
	connect(m_worker, SIGNAL(finished()), m_worker, SLOT(deleteLater()));
	connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));

	m_thread->start();
}

SqliteHistory::~SqliteHistory()
{
	m_worker->shutdown();
	m_thread->wait();

	delete m_worker;
	delete m_thread;
}

void SqliteHistory::store(const Message &message)
{
	if (!message.chatUnit())
		return;

	auto contactInfo = info(message.chatUnit());

	m_worker->runJob([message, contactInfo] () {
		QSqlQuery query;

		query.prepare(QStringLiteral("INSERT INTO qutim_history (account, protocol, contact, year, month, day, time, incoming, message, html, type, sendername) "
									 "VALUES (:account, :protocol, :contact, :year, :month, :day, :time, :incoming, :message, :html, :type, :sendername)"));

		query.bindValue(QStringLiteral(":account"), contactInfo.account);
		query.bindValue(QStringLiteral(":protocol"), contactInfo.protocol);
		query.bindValue(QStringLiteral(":contact"), contactInfo.contact);

		query.bindValue(QStringLiteral(":year"), message.time().date().year());
		query.bindValue(QStringLiteral(":month"), message.time().date().month());
		query.bindValue(QStringLiteral(":day"), message.time().date().day());
		query.bindValue(QStringLiteral(":time"), message.time().toTime_t());

		query.bindValue(QStringLiteral(":incoming"), message.isIncoming());
		query.bindValue(QStringLiteral(":message"), message.text());
		query.bindValue(QStringLiteral(":html"), message.html());

		SqliteWorker::MessageTypes type = SqliteWorker::Message;
		if(message.property("topic", false))
			type |= SqliteWorker::Topic;
		if(message.property("service", false))
			type |= SqliteWorker::Service;

		query.bindValue(QStringLiteral(":type"), static_cast<int>(type));
		query.bindValue(QStringLiteral(":sendername"), message.property("senderName", QString()));
		query.exec();
	});
}

AsyncResult<MessageList> SqliteHistory::read(const ContactInfo &info, const QDateTime &from, const QDateTime &to, int max_num)
{
	AsyncResultHandler<MessageList> handler;

	m_worker->runJob([info, from, to, max_num, handler] () {
		QSqlQuery query;

		QString queryString = "SELECT time, incoming, message, html, type, sendername FROM qutim_history WHERE protocol = :protocol "
							  "AND account = :account "
							  "AND contact = :contact ";
		if(from.isValid())
			queryString += "AND time >= :time_from ";

		if(to.isValid())
			queryString += "AND time <= :time_to ";

		queryString += "ORDER BY time DESC ";

		if(max_num != -1)
			queryString += "LIMIT :max";
		query.prepare(queryString);

		if(from.isValid())
			query.bindValue(QStringLiteral(":time_from"), from.toTime_t());
		if(to.isValid())
			query.bindValue(QStringLiteral(":time_to"), to.toTime_t());

		qDebug() << "Trying to read messages";

		query.bindValue(QStringLiteral(":account"), info.account);
		query.bindValue(QStringLiteral(":protocol"), info.protocol);
		query.bindValue(QStringLiteral(":contact"), info.contact);

		query.bindValue(QStringLiteral(":max"), max_num);

		query.exec();

		auto error = query.lastError();
		if (error.isValid()) {
			qDebug() << "Error type: " << error.type();
			qDebug() << "Database: " << error.databaseText();
			qDebug() << "Driver: " << error.driverText();
		}

		MessageList items;

		while(query.next()) {
			Message msg;
			msg.setTime(QDateTime::fromTime_t(query.value(0).toUInt()));
			msg.setIncoming(query.value(1).toBool());
			msg.setText(query.value(2).toString());
			msg.setHtml(query.value(3).toString());

			if(!query.value(5).isNull())
				msg.setProperty("senderName", query.value(5).toString());

			SqliteWorker::MessageTypes type = static_cast<SqliteWorker::MessageTypes>(query.value(4).toInt());

			if(type.testFlag(SqliteWorker::Topic))
				msg.setProperty("topic", true);

			if(type.testFlag(SqliteWorker::Service))
				msg.setProperty("service", true);

			items.append(msg);
		}

		std::stable_sort(items.begin(), items.end(), [](const Message &left, const Message &right) {
			return left.time() < right.time();
		});

		handler.handle(items);
	});

	return handler.result();
}

AsyncResult<QVector<History::AccountInfo>> SqliteHistory::accounts()
{
	AsyncResultHandler<QVector<AccountInfo>> handler;

	m_worker->runJob([handler] () {
		QVector<AccountInfo> result;

		QSqlQuery query;
		query.prepare(QStringLiteral("SELECT DISTINCT account, protocol FROM qutim_history"));
		query.exec();
		qDebug() << "accounts()";

		while(query.next()) {
			AccountInfo info;
			info.account = query.value(0).toString();
			info.protocol = query.value(1).toString();
			qDebug() << "AccountInfo" << info.protocol << info.account;
			result << info;
		}

		handler.handle(result);
	});

	return handler.result();
}

AsyncResult<QVector<History::ContactInfo>> SqliteHistory::contacts(const AccountInfo &account)
{
	AsyncResultHandler<QVector<ContactInfo>> handler;

	m_worker->runJob([handler, account] () {
		QVector<ContactInfo> result;

		qDebug() << "contacts()";
		QSqlQuery query;
		query.prepare(QStringLiteral("SELECT DISTINCT contact FROM qutim_history WHERE account = :account AND protocol = :protocol"));
		query.bindValue(QStringLiteral(":account"), account.account);
		query.bindValue(QStringLiteral(":protocol"), account.protocol);
		query.exec();

		qDebug() << query.executedQuery();
		qDebug() << "account" << account.account;
		qDebug() << "protocol" << account.protocol;
		auto error = query.lastError();
		if (error.isValid()) {
			qDebug() << "Error type: " << error.type();
			qDebug() << "Database: " << error.databaseText();
			qDebug() << "Driver: " << error.driverText();
		}


		while(query.next()) {
			ContactInfo info;
			info.account = account.account;
			info.protocol = account.protocol;
			info.contact = query.value(0).toString();

			qDebug() << info.contact;
			result << info;
		}

		handler.handle(result);
	});

	return handler.result();
}

AsyncResult<QList<QDate>> SqliteHistory::months(const ContactInfo &contact, const QString &needle)
{
	AsyncResultHandler<QList<QDate>> handler;

	m_worker->runJob([handler, contact, needle] () {
		QList<QDate> result;

		QString queryString = QStringLiteral("SELECT DISTINCT year, month FROM qutim_history WHERE account = :account "
							  "AND protocol = :protocol "
							  "AND contact = :contact ");

		if(!needle.isEmpty())
			queryString += QStringLiteral("AND message LIKE :needle ESCAPE '@'");
		QSqlQuery query;
		query.prepare(queryString);
		query.bindValue(QStringLiteral(":account"), contact.account);
		query.bindValue(QStringLiteral(":protocol"), contact.protocol);
		query.bindValue(QStringLiteral(":contact"), contact.contact);
		if(!needle.isEmpty())
			query.bindValue(QStringLiteral(":needle"), QLatin1Char('%') + SqliteWorker::escapeSqliteLike(needle) + QLatin1Char('%'));
		query.exec();
		qDebug() << "months()";

		while(query.next()) {
			int year = query.value(0).toInt();
			int month = query.value(1).toInt();

			qDebug() << QDate(year, month, 1);
			result << QDate(year, month, 1);
		}

		std::sort(result.begin(), result.end());

		handler.handle(result);
	});

	return handler.result();
}

AsyncResult<QList<QDate>> SqliteHistory::dates(const ContactInfo &contact, const QDate &month, const QString &needle)
{
	AsyncResultHandler<QList<QDate>> handler;

	m_worker->runJob([handler, contact, month, needle] () {
		QList<QDate> result;

		QString queryString = QStringLiteral("SELECT DISTINCT day FROM qutim_history WHERE account = :account "
							  "AND protocol = :protocol "
							  "AND contact = :contact "
							  "AND year = :year "
							  "AND month = :month ");
		if(!needle.isEmpty())
			queryString += QStringLiteral("AND message LIKE :needle ESCAPE '@'");

		QSqlQuery query;
		query.prepare(queryString);
		query.bindValue(QStringLiteral(":account"), contact.account);
		query.bindValue(QStringLiteral(":protocol"), contact.protocol);
		query.bindValue(QStringLiteral(":contact"), contact.contact);
		query.bindValue(QStringLiteral(":year"), month.year());
		query.bindValue(QStringLiteral(":month"), month.month());
		if(!needle.isEmpty())
			query.bindValue(QStringLiteral(":needle"), QLatin1Char('%') + SqliteWorker::escapeSqliteLike(needle) + QLatin1Char('%'));
		query.exec();

		while(query.next()) {
			result << QDate(month.year(), month.month(), query.value(0).toInt());
		}

		std::sort(result.begin(), result.end());

		handler.handle(result);
	});

	return handler.result();
}

void SqliteHistory::errorHandler(const QString &error)
{
	qDebug() << error;
}

void SqliteWorker::prepareDb()
{
	QString dbScheme = QStringLiteral("CREATE TABLE IF NOT EXISTS qutim_history ("
					   "`id` INTEGER PRIMARY KEY AUTOINCREMENT, "
					   "`account` TEXT NOT NULL, "
					   "`protocol` TEXT NOT NULL, "
					   "`contact` TEXT NOT NULL, "
					   "`year` INTEGER(2) NOT NULL, "
					   "`month` INTEGER(1) NOT NULL, "
					   "`day` INTEGER(1) NOT NULL, "
					   "`time` TIMESTAMP NOT NULL, "
					   "`incoming` INTEGER(1) NOT NULL, "
					   "`message` TEXT NOT NULL, "
					   "`html` TEXT NOT NULL, "
					   "`type` INTEGER(1) NOT NULL, "
					   "`sendername` TEXT DEFAULT NULL"
					   ");");

	QSqlQuery res = m_db.exec(dbScheme);
	auto error = res.lastError();
	if(error.isValid())
		qDebug() << "Error during db init:" << error;

	QString dbMigrations = QStringLiteral("CREATE TABLE IF NOT EXISTS qutim_history_version ("
						   "`key` VARCHAR(32) NOT NULL, "
						   "`value` INTEGER(1) NOT NULL, "
						   "primary key (`key`)"
						   ");");
	QSqlQuery migrationsResult = m_db.exec(dbMigrations);
	auto migrationsError = migrationsResult.lastError();
	if(migrationsError.isValid())
		qDebug() << "Error during creation of qutim_history_version:" << error;

	qDebug() << "Checking version of database...";

	QSqlQuery query;
	query.prepare(QStringLiteral("SELECT value FROM qutim_history_version WHERE key = :key LIMIT 1"));
	query.bindValue(QStringLiteral(":key"), QStringLiteral("sqlitehistory"));
	query.exec();

	if(query.first()) {
		int version = query.value(0).toInt();
		if(version == currentVersion()) {
			qDebug() << "No migration needed";
			return;
		} else if(version > currentVersion()) {
			qDebug() << "Current version of sqlitehistory plugin is" << currentVersion();
			qDebug() << "But qutim.sqlite version = " << version;
			qFatal("qutIM sqlite database is older than plugin. Cannot proceed. Please upgrade qutIM to last version");
		} else if(version < currentVersion()) {
			qDebug() << "Database older than plugin, executing migrations";

			for(int i = version + 1; i < currentVersion(); ++i)
				makeMigration(i);
		}
	} else {
		qDebug() << "First run, inserting current version";
		QSqlQuery query;

		query.prepare(QStringLiteral("INSERT INTO qutim_history_version (key, value) "
					  "VALUES (:key, :value)"));
		query.bindValue(QStringLiteral(":key"), QStringLiteral("sqlitehistory"));
		query.bindValue(QStringLiteral(":value"), currentVersion());
		query.exec();

		qDebug() << "No migration needed";
	}
}

void SqliteWorker::exec()
{
	forever {
		m_queueLock.lock();
		if(m_queue.isEmpty()) {
			m_runningLock.lock();
			m_isRunning = false;
			m_runningLock.unlock();

			m_queueLock.unlock();
			break;
		}

		auto f = m_queue.dequeue();
		m_queueLock.unlock();

		f();
	}
}

void SqliteWorker::runJob(std::function<void ()> job)
{
	m_queueLock.lock();
	m_queue.enqueue(std::move(job));
	m_queueLock.unlock();

	m_runningLock.lock();
	if(!m_isRunning) {
		m_isRunning = true;
		m_runningLock.unlock();
		QTimer::singleShot(0, this, SLOT(exec()));
	} else
		m_runningLock.unlock();
}

void SqliteWorker::shutdown()
{
	emit finished();
}

/**
 * Escapes string to use in LIKE query. You need to add ESCAPE '@' to end of your query
 */
QString SqliteWorker::escapeSqliteLike(const QString &str)
{
	QString escaped = str;
	escaped.replace(QLatin1Char('@'), QStringLiteral("@@"));
	escaped.replace(QLatin1Char('_'), QStringLiteral("@_"));
	escaped.replace(QLatin1Char('%'), QStringLiteral("@%"));

	return escaped;
}

void SqliteWorker::process()
{
	m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
	QDir history_dir = SystemInfo::getDir(SystemInfo::HistoryDir);
	QString pathToHistory = history_dir.absolutePath() + QDir::separator() + "qutim.sqlite";
	qDebug() << "Trying to open database" << pathToHistory;
	m_db.setDatabaseName(pathToHistory);
	bool openSuccess = m_db.open();

	if(!openSuccess) {
		qDebug() << m_db.lastError();
		qFatal("Cannot open sqlite database");
	} else
		qDebug() << "Database opened!";

	prepareDb();

	m_runningLock.lock();
	m_isRunning = true;
	m_runningLock.unlock();
	exec();
}

void SqliteWorker::makeMigration(int version)
{
	qDebug() << "Executing migration to version" << version;
	m_db.transaction();
	QSqlQuery query;

	switch(version) {
	case 0:
		query.prepare(QStringLiteral("ALTER TABLE qutim_history ADD COLUMN `sendername` TEXT DEFAULT NULL"));
		break;
	default:
		qFatal("Unhandled migration to version %i! Seems like a bug", version);
		break;
	}

	query.exec();

	auto error = query.lastError();
	if(error.isValid()) {
		qDebug() << "Error during migration from" << version - 1 << "to" << version;
		qDebug() << "Error type: " << error.type();
		qDebug() << "Database: " << error.databaseText();
		qDebug() << "Driver: " << error.driverText();

		m_db.rollback();
		qFatal("Exiting...");
	} else {
		qDebug() << "Successful migration to version" << version;
		QSqlQuery q;
		q.prepare(QStringLiteral("UPDATE qutim_history_version SET value = :value WHERE key = :key"));
		q.bindValue(QStringLiteral(":key"), QStringLiteral("sqlitehistory"));
		q.bindValue(QStringLiteral(":value"), version);
		q.exec();

		m_db.commit();
	}
}

}

