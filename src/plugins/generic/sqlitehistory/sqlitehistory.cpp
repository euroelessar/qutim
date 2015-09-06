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
}

void SqliteHistory::store(const Message &message)
{
	if (!message.chatUnit())
		return;

	auto contactInfo = info(message.chatUnit());

	m_worker->runJob([message, contactInfo] () {
		QSqlQuery query;

		query.prepare("INSERT INTO qutim_history (account, protocol, contact, year, month, day, time, incoming, message, html, type, sendername) "
					  "VALUES (:account, :protocol, :contact, :year, :month, :day, :time, :incoming, :message, :html, :type, :sendername)");

		query.bindValue(":account", contactInfo.account);
		query.bindValue(":protocol", contactInfo.protocol);
		query.bindValue(":contact", contactInfo.contact);

		query.bindValue(":year", message.time().date().year());
		query.bindValue(":month", message.time().date().month());
		query.bindValue(":day", message.time().date().day());
		query.bindValue(":time", message.time().toTime_t());

		query.bindValue(":incoming", message.isIncoming());
		query.bindValue(":message", message.text());
		query.bindValue(":html", message.html());

		SqliteWorker::MessageTypes type = SqliteWorker::Message;
		if(message.property("topic", false))
			type |= SqliteWorker::Topic;
		if(message.property("service", false))
			type |= SqliteWorker::Service;

		query.bindValue(":type", static_cast<int>(type));
		query.bindValue(":sendername", message.property("senderName", QString()));
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
			query.bindValue(":time_from", from.toTime_t());
		if(to.isValid())
			query.bindValue(":time_to", to.toTime_t());

		qDebug() << "Trying to read messages";

		query.bindValue(":account", info.account);
		query.bindValue(":protocol", info.protocol);
		query.bindValue(":contact", info.contact);

		query.bindValue(":max", max_num);

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
		query.prepare("SELECT DISTINCT account, protocol FROM qutim_history");
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
		query.prepare("SELECT DISTINCT contact FROM qutim_history WHERE account = :account AND protocol = :protocol");
		query.bindValue(":account", account.account);
		query.bindValue(":protocol", account.protocol);
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

		QString queryString = "SELECT DISTINCT year, month FROM qutim_history WHERE account = :account "
						   "AND protocol = :protocol "
						   "AND contact = :contact ";

		if(!needle.isEmpty())
			queryString += "AND message LIKE :needle ESCAPE '@'";
		QSqlQuery query;
		query.prepare(queryString);
		query.bindValue(":account", contact.account);
		query.bindValue(":protocol", contact.protocol);
		query.bindValue(":contact", contact.contact);
		if(!needle.isEmpty())
			query.bindValue(":needle", QLatin1Char('%') + SqliteWorker::escapeSqliteLike(needle) + QLatin1Char('%'));
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

		QString queryString = "SELECT DISTINCT day FROM qutim_history WHERE account = :account "
							  "AND protocol = :protocol "
							  "AND contact = :contact "
							  "AND year = :year "
							  "AND month = :month ";
		if(!needle.isEmpty())
			queryString += "AND message LIKE :needle ESCAPE '@'";

		QSqlQuery query;
		query.prepare(queryString);
		query.bindValue(":account", contact.account);
		query.bindValue(":protocol", contact.protocol);
		query.bindValue(":contact", contact.contact);
		query.bindValue(":year", month.year());
		query.bindValue(":month", month.month());
		if(!needle.isEmpty())
			query.bindValue(":needle", QLatin1Char('%') + SqliteWorker::escapeSqliteLike(needle) + QLatin1Char('%'));
		query.exec();

		while(query.next()) {
			result << QDate(month.year(), month.month(), query.value(0).toInt());
		}

		std::sort(result.begin(), result.end());

		handler.handle(result);
	});

	return handler.result();
}

void SqliteWorker::prepareDb()
{
	QString dbScheme = "CREATE TABLE IF NOT EXISTS qutim_history ("
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
					   ");";

	QSqlQuery res = m_db.exec(dbScheme);
	auto error = res.lastError();
	if(error.isValid())
		qDebug() << "Error during db init:" << error;

	QString dbMigrations = "CREATE TABLE IF NOT EXISTS qutim_history_version ("
						   "`key` VARCHAR(32) NOT NULL, "
						   "`value` INTEGER(1) NOT NULL, "
						   "primary key (`key`)"
						   ");";
	QSqlQuery migrationsResult = m_db.exec(dbMigrations);
	auto migrationsError = migrationsResult.lastError();
	if(migrationsError.isValid())
		qDebug() << "Error during creation of qutim_history_version:" << error;

	qDebug() << "Checking version of database...";

	QSqlQuery query;
	query.prepare("SELECT value FROM qutim_history_version WHERE key = :key LIMIT 1");
	query.bindValue(":key", "sqlitehistory");
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

		query.prepare("INSERT INTO qutim_history_version (key, value) "
					  "VALUES (:key, :value)");
		query.bindValue(":key", "sqlitehistory");
		query.bindValue(":value", currentVersion());
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
		exec();
	}

}

/**
 * Escapes string to use in LIKE query. You need to add ESCAPE '@' to end of your query
 */
QString SqliteWorker::escapeSqliteLike(const QString &str)
{
	QString escaped = str;
	escaped.replace('@', "@@");
	escaped.replace('_', "@_");
	escaped.replace('%', "@%");

	return escaped;
}

void SqliteWorker::process()
{
	m_db = QSqlDatabase::addDatabase("QSQLITE");
	QDir history_dir = SystemInfo::getDir(SystemInfo::HistoryDir);
	QString pathToHistory = history_dir.absolutePath() + QDir::separator() + "qutim.sqlite";
	qDebug() << "Trying to open database" << pathToHistory;
	m_db.setDatabaseName(pathToHistory);
	bool openSuccess = m_db.open();

	if(!openSuccess)
		qDebug() << m_db.lastError();
	else
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
		query.prepare("ALTER TABLE qutim_history ADD COLUMN `sendername` TEXT DEFAULT NULL");
		break;
	default:
		qFatal("Unhandled migration to version %i! Seems like a bug", version);
		break;
	}

	query.exec();

	m_db.commit();

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
		q.prepare("UPDATE qutim_history_version SET value = :value WHERE key = :key");
		q.bindValue(":key", "sqlitehistory");
		q.bindValue(":value", version);
		q.exec();
	}
}

}

