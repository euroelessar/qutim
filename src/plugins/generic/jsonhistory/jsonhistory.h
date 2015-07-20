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

#ifndef JSONHISTORY_H
#define JSONHISTORY_H

#include <qutim/history.h>
#include <QRunnable>
#include <QDir>
#include <QLinkedList>
#include <QPointer>
#include <QMutex>
#include <QQueue>

using namespace qutim_sdk_0_3;

namespace Core
{
class HistoryWindow;

class JsonHistoryScope
{
public:
	typedef QSharedPointer<JsonHistoryScope> Ptr;

	uint findEnd(QFile &file);
	QString getFileName(const Message &message) const;
	QString getFileName(const History::ContactInfo &info, const QDate &time) const;
	QDir getAccountDir(const History::AccountInfo &info) const;

	struct EndValue
	{
		EndValue(const QDateTime &t, uint e) : lastModified(t), end(e) {}
		QDateTime lastModified;
		uint end;
	};

	typedef QHash<QString, EndValue> EndCache;
	bool hasJobRunnable;
	EndCache cache;

	QMutex queueLock;
	QQueue< std::function<void ()> > queue;
};

class JsonHistoryJob : public QRunnable
{
public:
	JsonHistoryJob(JsonHistoryScope::Ptr scope);
	void run() override;

private:
	JsonHistoryScope::Ptr d;
};

class JsonHistory : public History
{
	Q_OBJECT
public:
	JsonHistory();
	virtual ~JsonHistory();

	void store(const Message &message) override;
	AsyncResult<MessageList> read(const ContactInfo &info, const QDateTime &from, const QDateTime &to, int max_num) override;
	AsyncResult<QVector<AccountInfo>> accounts() override;
	AsyncResult<QVector<ContactInfo>> contacts(const AccountInfo &account) override;
	AsyncResult<QList<QDate>> months(const ContactInfo &contact, const QString &needle) override;
	AsyncResult<QList<QDate>> dates(const ContactInfo &contact, const QDate &month, const QString &needle) override;
	void showHistory(const ChatUnit *unit) override;

	static QString quote(const QString &str);
	static QString unquote(const QString &str);

private slots:
	void onHistoryActionTriggered(QObject *object);
private:
	JsonHistoryScope::Ptr m_scope;
	QPointer<HistoryWindow> m_historyWindow;
};
}

#endif // JSONHISTORY_H

