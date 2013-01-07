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

#include "plugin_p.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <QDir>
#include <QResource>
#include <QStringBuilder>
#include "json.h"

namespace qutim_sdk_0_3
{
typedef QHash<QString, PersonInfoData::Ptr> PersonInfoMap;
Q_GLOBAL_STATIC(PersonInfoMap, personInfoMap)

PersonInfoData::PersonInfoData(const QString &ocs) : ocsUsername(ocs)
{
	if (!ocs.isEmpty()) {
		QVariantMap info = data();
		name = LocalizedString("Author", info.value(QLatin1String("name")).toByteArray());
		task = LocalizedString("Task", info.value(QLatin1String("task")).toByteArray());
		email = info.value(QLatin1String("email")).toString();
		web = info.value(QLatin1String("web")).toString();
	}
}

PersonInfoData::PersonInfoData(const PersonInfoData &other)
		: QSharedData(other), name(other.name), task(other.task), email(other.email), web(other.web)
{
}

static QVariantMap qutim_resource_open(QResource &res)
{
	QByteArray buffer;
	int size = res.size();
	const uchar *data = res.data();
	if (res.isCompressed()) {
		buffer = qUncompress(res.data(), size);
		size = buffer.size();
		data = reinterpret_cast<const uchar*>(buffer.constData());
	}
	QVariant result;
	Json::parseRecord(result, data, &size);
	return result.toMap();
}

QVariantMap PersonInfoData::data() const
{
	QResource res(QLatin1Literal(":/devels/") % ocsUsername % QLatin1Literal(".json"));
	if (!res.isValid())
		res.setFileName(QLatin1Literal(":/contributers/") % ocsUsername % QLatin1Literal(".json"));
	return qutim_resource_open(res);
}

PersonInfo::PersonInfo(const LocalizedString &name, const LocalizedString &task, const QString &email, const QString &web)
{
	d = new PersonInfoData;
	setName(name);
	setTask(task);
	setEmail(email);
	setWeb(web);
}

PersonInfo::PersonInfo(const QString &ocsUsername)
{
	d = personInfoMap()->value(ocsUsername);
	if (!d) {
		d = new PersonInfoData(ocsUsername);
		personInfoMap()->insert(ocsUsername, d);
	}
}

PersonInfo::PersonInfo(const PersonInfo &other) : d(other.d)
{
}

PersonInfo::PersonInfo(const QSharedDataPointer<PersonInfoData> &p) : d(p)
{
}

PersonInfo::~PersonInfo()
{
}

PersonInfo &PersonInfo::operator =(const PersonInfo &other)
{
	d = other.d;
	return *this;
}

PersonInfo &PersonInfo::setName(const LocalizedString &name)
{
	if (d->ocsUsername.isEmpty())
		d->name = name;
	return *this;
}

PersonInfo &PersonInfo::setTask(const LocalizedString &task)
{
	if (d->ocsUsername.isEmpty())
		d->task = task;
	return *this;
}

PersonInfo &PersonInfo::setEmail(const QString &email)
{
	if (d->ocsUsername.isEmpty())
		d->email = email;
	return *this;
}

PersonInfo &PersonInfo::setWeb(const QString &web)
{
	if (d->ocsUsername.isEmpty())
		d->web = web;
	return *this;
}

LocalizedString PersonInfo::name() const
{
	return d->name;
}

LocalizedString PersonInfo::task() const
{
	return d->task;
}

QString PersonInfo::email() const
{
	return d->email;
}

QString PersonInfo::web() const
{
	return d->web;
}

QString PersonInfo::ocsUsername() const
{
	return d->ocsUsername;
}

typedef QPair<PersonInfo, int> PersonIntPair;
typedef QHash<QString, PersonIntPair> StringPersonHash;

inline bool personLessThen(const PersonIntPair &a, const PersonIntPair &b)
{
	return a.second > b.second || (a.second == b.second && a.first.name() < b.first.name());
}

QList<PersonInfo> PersonInfo::authors()
{
	// May be we should use QMap and get result finally from it withour QVector and qSort?
	StringPersonHash authors;
	StringPersonHash::iterator it;
    foreach (QPointer<Plugin> plugin, pluginsList()) {
        foreach (const PersonInfo &person, plugin.data()->info().authors()) {
			it = authors.find(person.name());
			if (it == authors.end())
				it = authors.insert(person.name(), qMakePair(person, 0));
            it.value().second += plugin.data()->avaiableExtensions().size();
		}
	}
	QVector<PersonIntPair> persons;
	persons.reserve(authors.size());
	it = authors.begin();
	for (; it != authors.end(); it++)
		persons.append(it.value());
	QDir contributersDir(QLatin1String(":/contributers/"));
	QStringList contributers = contributersDir.entryList(QStringList(QLatin1String("*.json")));
	for (int i = 0; i < contributers.size(); ++i) {
		contributers[i].chop(5); // ".json".length()
		persons << qMakePair(PersonInfo(contributers[i]), 0);
	}
	qSort(persons.begin(), persons.end(), personLessThen);
	QList<PersonInfo> result;
	for (int i = 0; i < persons.size(); i++)
		result << persons.at(i).first;
	return result;
}

QList<PersonInfo> PersonInfo::translators()
{
	LocalizedString names = QT_TRANSLATE_NOOP("TRANSLATORS", "Your names");
	LocalizedString emails = QT_TRANSLATE_NOOP("TRANSLATORS", "Your emails");
	LocalizedString webs = QT_TRANSLATE_NOOP("TRANSLATORS", "Your web addresses");
	QString localizedNames = names.toString();
	QList<PersonInfo> persons;
	LocalizedString task = QT_TRANSLATE_NOOP("Task", "Translator");
	if (localizedNames != QLatin1String(names.original())) {
		QString localizedEmails = emails.toString();
		if (localizedEmails == QLatin1String(emails.original()))
			localizedEmails.clear();
		QString localizedWebs = webs.toString();
		if (localizedWebs == QLatin1String(webs.original()))
			localizedWebs.clear();
		QStringList nameList = localizedNames.split(QLatin1Char(','));
		QStringList emailList = localizedEmails.split(QLatin1Char(','), QString::KeepEmptyParts);
		QStringList webList = localizedWebs.split(QLatin1Char(','), QString::KeepEmptyParts);
		for (int i = 0; i < nameList.size(); i++) {
			persons << PersonInfo(nameList.at(i).toUtf8(),
			                      task,
			                      emailList.value(i),
			                      webList.value(i));
		}
	}
	return persons;
}

PersonInfo::Data *PersonInfo::data()
{
	return const_cast<Data *>(d.constData());
}
}

