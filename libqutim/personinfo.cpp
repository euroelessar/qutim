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

namespace qutim_sdk_0_3
{
	PersonInfoData::PersonInfoData()
	{
	}

	PersonInfoData::PersonInfoData(const PersonInfoData &other)
			: QSharedData(other), name(other.name), task(other.task), email(other.email), web(other.web)
	{
	}

	PersonInfo::PersonInfo(const LocalizedString &name, const LocalizedString &task, const QString &email, const QString &web)
	{
		d = new PersonInfoData;
		setName(name);
		setTask(task);
		setEmail(email);
		setWeb(web);
	}

	PersonInfo::PersonInfo(const PersonInfo &other) : d(other.d)
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
		return d->name;
	}

	LocalizedString PersonInfo::task() const
	{
		return d->task;
	}

	const QString &PersonInfo::email() const
	{
		return d->email;
	}

	const QString &PersonInfo::web() const
	{
		return d->web;
	}

	PersonInfo::Data *PersonInfo::data()
	{
		return const_cast<Data *>(d.constData());
	}
}
