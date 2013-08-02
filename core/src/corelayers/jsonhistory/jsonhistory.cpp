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

#include "jsonhistory.h"
#include <qutim/chatunit.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/systeminfo.h>
#include <qutim/json.h>
#include <QStringBuilder>
#include <QThreadPool>
#include "historywindow.h"
#include <qutim/icon.h>
#include <qutim/debug.h>
//#include <QElapsedTimer>

namespace Core
{
JsonHistoryRunnable::JsonHistoryRunnable(JsonHistoryScope *scope) : d(scope)
{
	d->hasRunnable = true;
	setAutoDelete(true);
}

void JsonHistoryRunnable::run()
{
	forever {
		d->mutex.lock();
		if (d->queue.isEmpty()) {
			d->hasRunnable = false;
			d->mutex.unlock();
			break;
		}
		QLinkedList<Message>::Iterator it = d->queue.begin();
		QDate date = it->time().date();
		const ChatUnit *unit = it->chatUnit()->getHistoryUnit();
		QList<Message> queue;
		while (it != d->queue.end()) {
			if (unit == it->chatUnit()->getHistoryUnit()
			        && date == it->time().date()) {
				queue << *it;
				d->queue.erase(it++);
			} else {
				++it;
			}
		}
		d->mutex.unlock();
		
		QString fileName = d->getAccountDir(unit).filePath(d->getFileName(queue.first()));
		QFile file(fileName);
		QDateTime lastModified = QFileInfo(fileName).lastModified();
		bool new_file = !file.exists();
		if(!file.open(QIODevice::ReadWrite))
			return;
		if(new_file) {
			file.write("[\n");
		} else {
			JsonHistoryScope::EndCache::iterator it = d->cache.find(fileName);
			uint end;
			if (it != d->cache.end() && it->lastModified == lastModified)
				end = it->end;
			else
				end = d->findEnd(file);
			file.resize(end);
			file.seek(end);
			file.write(",\n");
		}
		for (int i = 0; i < queue.size(); ++i) {
			const Message &message = queue.at(i);
			if (i > 0)
				file.write(",\n");
			file.write(" {\n");
			foreach(const QByteArray &name, message.dynamicPropertyNames()) {
				QByteArray data;
				if(!Json::generate(data, message.property(name), 2))
					continue;
				file.write("  ");
				file.write(Json::quote(QString::fromUtf8(name)).toUtf8());
				file.write(": ");
				file.write(data);
				file.write(",\n");
			}
			file.write("  \"datetime\": \"");
			QDateTime time = message.time();
			if(!time.isValid())
				time = QDateTime::currentDateTime();
			file.write(time.toString(Qt::ISODate).toLatin1());
			file.write("\",\n  \"in\": ");
			file.write(message.isIncoming() ? "true" : "false");
			file.write(",\n  \"text\": ");
			file.write(Json::quote(message.text()).toUtf8());
			file.write(",\n  \"html\": ");
			file.write(Json::quote(message.html()).toUtf8());
			file.write("\n }");
		}
		uint end = file.pos();
		file.write("\n]");
		file.close();
		lastModified = QFileInfo(fileName).lastModified();
		d->cache.insert(fileName, JsonHistoryScope::EndValue(lastModified, end));
	//	It will produce something like this:
	//	{
	//	 "datetime": "2009-06-20T01:42:22",
	//	 "type": 1,
	//	 "in": true,
	//	 "text": "some cool text"
	//	}
	}
}

void init(History *history)
{
	ActionGenerator *gen = new ActionGenerator(Icon("view-history"),
										QT_TRANSLATE_NOOP("Chat", "View History"),
										history,
										SLOT(onHistoryActionTriggered(QObject*)));
	gen->setType(ActionTypeChatButton|ActionTypeContactList);
	gen->setPriority(512);
	MenuController::addAction<ChatUnit>(gen);
}

JsonHistory::JsonHistory()
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		init(this);
	}
	m_scope.hasRunnable = false;
}

JsonHistory::~JsonHistory()
{
}

uint JsonHistoryScope::findEnd(QFile &file)
{
	int len = file.size();
	QByteArray data;
	uchar *fmap = file.map(0, file.size());
	if(!fmap)
	{
		data = file.readAll();
		fmap = (uchar *)data.constData();
	}
	uint end = file.size();
	const uchar *s = Json::skipBlanks(fmap, &len);
	uchar qch = *s;
	if(!s || (qch != '[' && qch != '{'))
	{
		if(data.isEmpty())
			file.unmap(fmap);
		return end;
	}
	qch = (qch == '{' ? '}' : ']');
	s++;
	len--;
	bool first = true;
	while(s)
	{
		s = Json::skipBlanks(s, &len);
		if(len < 2 || (s && *s == qch))
		{
			if(*(s-1) == '\n')
				s--;
			end = (uint)(s - fmap);
			break;
		}
		if(!s)
			break;
		if((!first && *s != ',') || (first && *s == ','))
			break;
		first = false;
		if(*s == ',')
		{
			s++;
			len--;
		}
		if(!(s = Json::skipRecord(s, &len)))
			break;
	}
	if(data.isEmpty())
		file.unmap(fmap);
	return end;
}

inline QString protocol(const ChatUnit *unit)
{
	const Account *acc = unit ? unit->account() : 0;
	const Protocol *proto = acc ? acc->protocol() : 0;
	return proto ? proto->id() : QLatin1String("unknown");
}

QString JsonHistoryScope::getFileName(const Message &message) const
{
	QDateTime time = message.time().isValid() ? message.time() : QDateTime::currentDateTime();
	QString fileName = JsonHistory::quote(message.chatUnit()->getHistoryUnit()->id());
	fileName += time.toString(QLatin1String(".yyyyMM.'json'"));
	return fileName;
}

QDir JsonHistoryScope::getAccountDir(const ChatUnit *unit) const
{
	QDir history_dir = SystemInfo::getDir(SystemInfo::HistoryDir);
	QString path = JsonHistory::quote(protocol(unit));
	path += QLatin1Char('.');
	if (unit->account())
		path += JsonHistory::quote(unit->account()->id());
	else
		path += QLatin1String("unknown");
	if(!history_dir.exists(path))
		history_dir.mkpath(path);
	return history_dir.filePath(path);
}

//	struct MethodLiveCounter
//	{
//		MethodLiveCounter(const char *m, qint64 &t) : method(m), time(t) { timer.start(); }
//		~MethodLiveCounter() { time += timer.elapsed(); qDebug() << method << "worked" << time << "ms"; }
//		QElapsedTimer timer;
//		const char *method;
//		qint64 &time;
//	};

void JsonHistory::store(const Message &message)
{
//		static qint64 t = 0;
//		MethodLiveCounter counter(Q_FUNC_INFO, t);
	if(!message.chatUnit())
		return;
	
#if 0
	QString fileName = getAccountDir(message.chatUnit()->getHistoryUnit()).filePath(getFileName(message));
	QFile file(fileName);
	QDateTime lastModified = QFileInfo(fileName).lastModified();
	bool new_file = !file.exists();
	if(!file.open(QIODevice::ReadWrite))
		return;
	if(new_file) {
		file.write("[\n");
	} else {
		EndCache::iterator it = m_cache.find(fileName);
		uint end;
		if (it != m_cache.end() && it->lastModified == lastModified)
			end = it->end;
		else
			end = findEnd(file);
		file.resize(end);
		file.seek(end);
		file.write(",\n");
	}
	file.write(" {\n");
	foreach(const QByteArray &name, message.dynamicPropertyNames())
	{
		QByteArray data;
		if(!Json::generate(data, message.property(name), 2))
			continue;
		file.write("  ");
		file.write(Json::quote(QString::fromUtf8(name)).toUtf8());
		file.write(": ");
		file.write(data);
		file.write(",\n");
	}
	file.write("  \"datetime\": \"");
	QDateTime time = message.time();
	if(!time.isValid())
		time = QDateTime::currentDateTime();
	file.write(time.toString(Qt::ISODate).toLatin1());
	file.write("\",\n  \"in\": ");
	file.write(message.isIncoming() ? "true" : "false");
	file.write(",\n  \"text\": ");
	file.write(Json::quote(message.text()).toUtf8());
	file.write("\n }");
	uint end = file.pos();
	file.write("\n]");
	file.close();
	lastModified = QFileInfo(fileName).lastModified();
	EndValue value = { lastModified, end };
	m_cache.insert(fileName, value);
//	It will produce something like this:
//	{
//	 "datetime": "2009-06-20T01:42:22",
//	 "type": 1,
//	 "in": true,
//	 "text": "some cool text"
//	}
#else
	QMutexLocker locker(&m_scope.mutex);
	m_scope.queue << message;
	if (!m_scope.hasRunnable)
		QThreadPool::globalInstance()->start(new JsonHistoryRunnable(&m_scope));
#endif
}

MessageList JsonHistory::read(const ChatUnit *unit, const QDateTime &from, const QDateTime &to, int max_num)
{
//		static qint64 t = 0;
//		MethodLiveCounter counter(Q_FUNC_INFO, t);
	MessageList items;
	if(!unit)
		return items;

	const ChatUnit *u = unit->getHistoryUnit();

	QDir dir = getAccountDir(u);
	QString filter = quote(u->id());
	filter += ".*.json";
	QStringList files = dir.entryList(QStringList() << filter, QDir::Readable | QDir::Files | QDir::NoDotAndDotDot,QDir::Name);
	if(files.isEmpty())
		return items;
	for(int i=files.size()-1; i>=0; i--)
	{
		QList<const uchar *> pointers;
		QFile file(dir.filePath(files[i]));
		if(!file.open(QIODevice::ReadOnly))
			continue;
		int len = file.size();
		QByteArray data;
		const uchar *fmap = file.map(0, file.size());
		if(!fmap)
		{
			data = file.readAll();
			fmap = (uchar *)data.constData();
		}
		const uchar *s = Json::skipBlanks(fmap, &len);
		uchar qch = *s;
		if(!s || (qch != '[' && qch != '{'))
			continue;
		qch = (qch == '{' ? '}' : ']');
		s++;
		len--;
		bool first = true;
		while(s)
		{
			s = Json::skipBlanks(s, &len);
			if(len < 2 || (s && *s == qch))
				break;
			if((!first && *s != ',') || (first && *s == ','))
				break;
			first = false;
			if(*s == ',')
			{
				s++;
				len--;
			}
			pointers.prepend(s);
			if(!(s = Json::skipRecord(s, &len)))
			{
				pointers.removeFirst();
				break;
			}
		}
		QVariant value;
		for(int i=0; i<pointers.size(); i++)
		{
			value.clear();
			s = pointers[i];
			len = file.size() + 1 - (s - fmap);
			Json::parseRecord(value, s, &len);
			QVariantMap message = value.toMap();
			Message item;
			QVariantMap::iterator it = message.begin();
			for(; it != message.end(); it++)
			{
				QString key = it.key();
				if(key == QLatin1String("datetime"))
					item.setTime(QDateTime::fromString(it.value().toString(), Qt::ISODate));
				else
					item.setProperty(key.toUtf8(), it.value());
			}
			if(item.time() >= to)
				continue;
			if(item.time() < from)
				return items;
			items.prepend(item);
			if((items.size() >= max_num) && (max_num != -1))
				return items;
		}
	}
	return items;
}

void JsonHistory::showHistory(const ChatUnit *unit)
{
	unit = unit->getHistoryUnit();
	if (m_historyWindow) {
		m_historyWindow.data()->setUnit(unit);
		m_historyWindow.data()->raise();
	}
	else {
		m_historyWindow = new Core::HistoryWindow(unit);
		m_historyWindow.data()->show();
	}
}

QString JsonHistory::quote(const QString &str)
{
	const static bool true_chars[128] =
	{// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
/* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0,
/* 4 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0,
/* 6 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0
	};
	QString result;
	result.reserve(str.size() * 2); // I hope it's enough in most cases
	const QChar *s = str.data();
	while(!s->isNull())
	{
		if(s->unicode() < 0xff && true_chars[s->unicode()])
			result += *s;
		else
		{
			result += '%';
			if(s->unicode() < 0x1000)
				result += '0';
			if(s->unicode() < 0x100)
				result += '0';
			if(s->unicode() < 0x10)
				result += '0';
			result += QString::number(s->unicode(), 16);
		}
		s++;
	}
	return result;
}

QString JsonHistory::unquote(const QString &str)
{
	QString result;
	bool ok = false;
	result.reserve(str.size()); // the worst variant
	const QChar *s = str.data();
	while(!s->isNull())
	{
		if(s->unicode() == L'%')
		{
			result += QChar(QString::fromRawData(++s, 4).toUShort(&ok, 16));
			s += 3;
		}
		else
			result += *s;
		s++;
	}
	return result;
}

void JsonHistory::onHistoryActionTriggered(QObject* object)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(object);
	showHistory(unit);
}

}

