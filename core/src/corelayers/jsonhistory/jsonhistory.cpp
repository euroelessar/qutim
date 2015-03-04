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
JsonHistoryStoreJob::JsonHistoryStoreJob(JsonHistoryScope::Ptr scope) : d(scope)
{
	d->hasRunnable = true;
	setAutoDelete(true);
}

void JsonHistoryStoreJob::run()
{
	forever {
		d->mutex.lock();
		if (d->queue.isEmpty()) {
			d->hasRunnable = false;
			d->mutex.unlock();
			break;
		}
        auto it = d->queue.begin();
        QDate date = it->second.time().date();
        auto firstContact = it->first;
        QList<QPair<History::ContactInfo, Message>> queue;
		while (it != d->queue.end()) {
            if (firstContact == it->first
                    && date == it->second.time().date()) {
				queue << *it;
				d->queue.erase(it++);
			} else {
				++it;
			}
		}
		d->mutex.unlock();
		
        QString fileName = d->getFileName(firstContact, date);
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
            const Message &message = queue.at(i).second;
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


JsonHistoryJob::JsonHistoryJob(const std::function<void ()> &handler)
    : m_handler(handler)
{
}

void JsonHistoryJob::run()
{
    m_handler();
}

template <typename Method>
static void runJob(Method method)
{
    JsonHistoryJob *job = new JsonHistoryJob(std::move(method));
    QThreadPool::globalInstance()->start(job);
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

JsonHistory::JsonHistory() : m_scope(new JsonHistoryScope)
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		init(this);
	}
    m_scope->hasRunnable = false;
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

QString JsonHistoryScope::getFileName(const Message &message) const
{
    return getFileName(History::info(message.chatUnit()), message.time().date());
}

QString JsonHistoryScope::getFileName(const History::ContactInfo &info, const QDate &time) const
{
    QDir accountDir = getAccountDir(info);
    QString fileName = JsonHistory::quote(info.contact);
    fileName += (time.isValid() ? time : QDate::currentDate())
            .toString(QStringLiteral(".yyyyMM.'json'"));
    return accountDir.filePath(fileName);
}

QDir JsonHistoryScope::getAccountDir(const History::AccountInfo &info) const
{
	QDir history_dir = SystemInfo::getDir(SystemInfo::HistoryDir);
    QString path = JsonHistory::quote(info.protocol);
    path += QLatin1Char('.');
    path += JsonHistory::quote(info.account);
	if(!history_dir.exists(path))
		history_dir.mkpath(path);
	return history_dir.filePath(path);
}

void JsonHistory::store(const Message &message)
{
    if (!message.chatUnit())
        return;

    QMutexLocker locker(&m_scope->mutex);
    m_scope->queue << qMakePair(info(message.chatUnit()), message);
    if (!m_scope->hasRunnable)
        QThreadPool::globalInstance()->start(new JsonHistoryStoreJob(m_scope));
}

AsyncResult<MessageList> JsonHistory::read(const ContactInfo &info, const QDateTime &from, const QDateTime &to, int max_num)
{
    AsyncResultHandler<MessageList> handler;
    auto scope = m_scope;

    runJob([scope, info, from, to, max_num, handler] () {
        QDir dir = scope->getAccountDir(info);
        QString filter = quote(info.contact);
        filter += ".*.json";

        MessageList items;
        QStringList files = dir.entryList(QStringList() << filter, QDir::Readable | QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        if (files.isEmpty()) {
            handler.handle(items);
            return;
        }
        for (int i = files.size() - 1; i >= 0; i--) {
            QList<const uchar *> pointers;
            QFile file(dir.filePath(files[i]));
            if (!file.open(QIODevice::ReadOnly))
                continue;
            int len = file.size();
            QByteArray data;
            const uchar *fmap = file.map(0, file.size());
            if (!fmap) {
                data = file.readAll();
                fmap = (uchar *)data.constData();
            }
            const uchar *s = Json::skipBlanks(fmap, &len);
            uchar qch = *s;
            if (!s || (qch != '[' && qch != '{'))
                continue;
            qch = (qch == '{' ? '}' : ']');
            s++;
            len--;
            bool first = true;
            while(s) {
                s = Json::skipBlanks(s, &len);
                if(len < 2 || (s && *s == qch))
                    break;
                if((!first && *s != ',') || (first && *s == ','))
                    break;
                first = false;
                if(*s == ',') {
                    s++;
                    len--;
                }
                pointers.prepend(s);
                if (!(s = Json::skipRecord(s, &len))) {
                    pointers.removeFirst();
                    break;
                }
            }
            QVariant value;
            for (int i = 0; i < pointers.size(); i++) {
                value.clear();
                s = pointers[i];
                len = file.size() + 1 - (s - fmap);
                Json::parseRecord(value, s, &len);
                QVariantMap message = value.toMap();
                Message item;
                QVariantMap::iterator it = message.begin();
                for (; it != message.end(); it++) {
                    QString key = it.key();
                    if(key == QLatin1String("datetime"))
                        item.setTime(QDateTime::fromString(it.value().toString(), Qt::ISODate));
                    else
                        item.setProperty(key.toUtf8(), it.value());
                }
                if (item.time() >= to)
                    continue;
                if (item.time() < from) {
                    handler.handle(items);
                    return;
                }
                items.prepend(item);
                if ((items.size() >= max_num) && (max_num != -1)) {
                    handler.handle(items);
                    return;
                }
            }
        }

        handler.handle(items);
    });

    return handler.result();
}

AsyncResult<QVector<History::AccountInfo>> JsonHistory::accounts()
{
    AsyncResultHandler<QVector<AccountInfo>> handler;

    runJob([handler] () {
        QVector<AccountInfo> result;

        QDir historyDir = SystemInfo::getDir(SystemInfo::HistoryDir);
        QStringList accounts = historyDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        const QStringList filter = QStringList() << QStringLiteral("*.*.json");

        foreach (QString account, accounts) {
            QDir account_dir = historyDir.filePath(account);
            if (account_dir.entryList(filter).isEmpty())
                continue;

            AccountInfo info;
            info.protocol = account.section(QStringLiteral("."), 0, 0);
            info.account = JsonHistory::unquote(account.section(QStringLiteral("."), 1));
            result << info;
        }

        handler.handle(result);
    });

    return handler.result();
}

AsyncResult<QVector<History::ContactInfo>> JsonHistory::contacts(const AccountInfo &account)
{
    AsyncResultHandler<QVector<ContactInfo>> handler;

    auto scope = m_scope;
    runJob([handler, scope, account] () {
        QVector<ContactInfo> result;
        QSet<QString> used;

        QDir accountDir = scope->getAccountDir(account);
        foreach (QString contact, accountDir.entryList(QDir::Files | QDir::NoDotAndDotDot)) {
            ContactInfo info;
            info.account = account.account;
            info.protocol = account.protocol;
            info.contact = unquote(contact.section(QStringLiteral("."), 0, -3));

            if (!used.contains(info.contact)) {
                used.insert(info.contact);
                result << info;
            }
        }

        handler.handle(result);
    });

    return handler.result();
}

AsyncResult<QList<QDate>> JsonHistory::months(const ContactInfo &contact, const QRegularExpression &regex)
{
    Q_UNUSED(regex);
    AsyncResultHandler<QList<QDate>> handler;

    auto scope = m_scope;
    runJob([handler, scope, contact] () {
        QList<QDate> result;
        QSet<QString> used;

        QDir accountDir = scope->getAccountDir(contact);
        QStringList filters = QStringList() << JsonHistory::quote(contact.contact) + QStringLiteral(".*");
        QStringList filesNames = accountDir.entryList(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

        foreach (const QString &fileName, filesNames) {
            QString date = fileName.section(QLatin1Char('.'), -2, -2 );
            if (date.length() != 6 || used.contains(date))
                continue;
            used.insert(date);

            int year = date.mid(0, 4).toInt();
            int month = date.mid(4, 2).toInt();

            result << QDate(year, month, 1);
        }

        std::sort(result.begin(), result.end());

        handler.handle(result);
    });

    return handler.result();
}

AsyncResult<QList<QDate>> JsonHistory::dates(const ContactInfo &contact, const QDate &month, const QRegularExpression &regex)
{
    AsyncResultHandler<QList<QDate>> handler;

    auto scope = m_scope;
    runJob([handler, scope, contact, month, regex] () {
        QSet<QDate> result;

        QFile file(scope->getFileName(contact, month));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            handler.handle(result.toList());
            return;
        }

        int len = file.size();
        QByteArray data;
        const uchar *fmap = file.map(0, file.size());
        if (!fmap) {
            data = file.readAll();
            fmap = (uchar *)data.constData();
        }
        const uchar *s = Json::skipBlanks(fmap, &len);
        QVariant val;
        uchar qch = *s;
        if (!s || (qch != '[' && qch != '{'))
            return;
        qch = (qch == '{' ? '}' : ']');
        s++;
        len--;
        bool first = true;
        while (s) {
            val.clear();
            s = Json::skipBlanks(s, &len);
            if (len < 2 || (s && *s == qch))
                break;
            if ((!first && *s != ',') || (first && *s == ','))
                break;
            first = false;
            if (*s == ',') {
                s++;
                len--;
            }
            if (!(s = Json::parseRecord(val, s, &len))) {
                break;
            } else {
                const QVariantMap message = val.toMap();
                const QDate date = QDateTime::fromString(message.value("datetime").toString(), Qt::ISODate).date();
                const QString text = message.value("text").toString();

                if (!regex.isValid() || text.contains(regex))
                    result.insert(date);
            }
        }

        QList<QDate> sortedResult = result.toList();
        std::sort(sortedResult.begin(), sortedResult.end());

        handler.handle(sortedResult);
    });

    return handler.result();
}

void JsonHistory::showHistory(const ChatUnit *unit)
{
    unit = unit->getHistoryUnit();
    if (m_historyWindow) {
        m_historyWindow.data()->setUnit(unit);
        m_historyWindow.data()->raise();
    } else {
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

