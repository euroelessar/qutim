/****************************************************************************
 *  personinfo.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef PERSONINFO_H
#define PERSONINFO_H

#include "libqutim_global.h"
#include "localizedstring.h"
#include <QSharedDataPointer>
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class PersonInfoData;

	class LIBQUTIM_EXPORT PersonInfo
	{
	public:
		PersonInfo(const LocalizedString &name = LocalizedString(),
				   const LocalizedString &task = LocalizedString(),
				   const QString &email = QString(), const QString &web = QString());
		PersonInfo(const PersonInfo &other);
		~PersonInfo();
		PersonInfo &operator =(const PersonInfo &other);
		PersonInfo &setName(const LocalizedString &name);
		PersonInfo &setTask(const LocalizedString &task);
		PersonInfo &setEmail(const QString &email);
		PersonInfo &setWeb(const QString &web);
		LocalizedString name() const;
		LocalizedString task() const;
		const QString &email() const;
		const QString &web() const;
	private:
		QSharedDataPointer<PersonInfoData> d;
	public:
		typedef PersonInfoData Data;
		Data *data();
	};
	/*
	 * Encoding is utf-8, language russian
	 * PersonInfo author = PersonInfo(QT_TRANSLATE_NOOP("Author", "Ivan Sidoroff"),
	 *								  QT_TRANSLATE_NOOP("Task", "Plugin author"),
	 *								  "v.sidorov@proger.ru",
	 *								  "http://proger.ru/v.sidorov");
	 * author.name(); // "Иван Сидоров"
	 * author.task(); // "Автор плагина"
	 */
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::PersonInfo)

#endif // PERSONINFO_H
