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
#include <QWeakPointer>
#include <QMutex>

using namespace qutim_sdk_0_3;

namespace Core
{
class HistoryWindow;
class JsonHistoryScope;

class JsonHistoryRunnable : public QRunnable
{
public:
	JsonHistoryRunnable(JsonHistoryScope *scope);
	virtual void run();
	
private:
	JsonHistoryScope *d;
};

class JsonHistoryScope
{
public:
	uint findEnd(QFile &file);
	QString getFileName(const Message &message) const;
	QDir getAccountDir(const ChatUnit *unit) const;
	
	struct EndValue
	{
		EndValue(const QDateTime &t, uint e) : lastModified(t), end(e) {}
		QDateTime lastModified;
		uint end;
	};
	
	typedef QHash<QString, EndValue> EndCache;
	bool hasRunnable;
	EndCache cache;
	QLinkedList<Message> queue;
	QMutex mutex;
};

class JsonHistory : public History
{
	Q_OBJECT
public:
	JsonHistory();
	virtual ~JsonHistory();
	uint findEnd(QFile &file) { return m_scope.findEnd(file); }
	virtual void store(const Message &message);
//		void flushMessages();
	virtual MessageList read(const ChatUnit *unit, const QDateTime &from, const QDateTime &to, int max_num);
	virtual void showHistory(const ChatUnit *unit);
	static QString quote(const QString &str);
	static QString unquote(const QString &str);
private slots:
	void onHistoryActionTriggered(QObject *object);
private:
	QString getFileName(const Message &message) const { return m_scope.getFileName(message); }
	QDir getAccountDir(const ChatUnit *unit) const { return m_scope.getAccountDir(unit); }
	
	JsonHistoryScope m_scope;
	QWeakPointer<HistoryWindow> m_historyWindow;
};
}

#endif // JSONHISTORY_H

