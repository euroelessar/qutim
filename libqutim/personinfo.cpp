/****************************************************************************
*  personinfo.cpp
*
*  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "plugin_p.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QHash>
#include <QResource>
#include <QStringBuilder>
#include "json.h"

namespace qutim_sdk_0_3
{
typedef QHash<QString, PersonInfoData::Ptr> PersonInfoMap;
Q_GLOBAL_STATIC(PersonInfoMap, personInfoMap)

PersonInfoData::PersonInfoData()
{
}

PersonInfoData::PersonInfoData(const PersonInfoData &other)
		: QSharedData(other), name(other.name), task(other.task), email(other.email), web(other.web)
{
}

QVariantMap qutim_resource_open(QResource &res)
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

QVariant PersonInfoData::data(const QString &key) const
{
	QResource res(QLatin1Literal(":/devels/") % ocsUsername % QLatin1Literal(".json"));
	QVariant value = qutim_resource_open(res).value(key);
	if (value.isNull() || !value.isValid()) {
		res.setLocale(QLocale::c());
		value = qutim_resource_open(res).value(key);
	}
	return value;
}

//PersonInfo PersonInfoData::unique(const PersonInfo &info)
//{
//	QString name = info.name();
//	if (name.isEmpty())
//		name = info.ocsUsername();
//	if (name.isEmpty())
//		return info;
//	PersonInfoData::Ptr data = personInfoMap()->value();
//	if (!data)
//		personInfoMap()->insert(name, info.d);
//	return PersonInfo(data);
//}

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
		d = new PersonInfoData;
		d->ocsUsername = ocsUsername;
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
	d->name = name;
	return *this;
}

PersonInfo &PersonInfo::setTask(const LocalizedString &task)
{
	d->task = task;
	return *this;
}

PersonInfo &PersonInfo::setEmail(const QString &email)
{
	d->email = email;
	return *this;
}

PersonInfo &PersonInfo::setWeb(const QString &web)
{
	d->web = web;
	return *this;
}

LocalizedString PersonInfo::name() const
{
	if (!d->ocsUsername.isEmpty()) {
		QString name = d->data(QLatin1String("name")).toString();
		return LocalizedString(QByteArray(), name.toUtf8());
	}
	return d->name;
}

LocalizedString PersonInfo::task() const
{
	if (!d->ocsUsername.isEmpty()) {
		QString name = d->data(QLatin1String("task")).toString();
		return LocalizedString(QByteArray(), name.toUtf8());
	}
	return d->task;
}

QString PersonInfo::email() const
{
	if (!d->ocsUsername.isEmpty())
		return d->data(QLatin1String("email")).toString();
	return d->email;
}

QString PersonInfo::web() const
{
	if (!d->ocsUsername.isEmpty())
		return d->data(QLatin1String("web")).toString();
	return d->web;
}

QString PersonInfo::ocsUsername() const
{
	return d->ocsUsername;
}

PersonInfo::Data *PersonInfo::data()
{
	return const_cast<Data *>(d.constData());
}
}
