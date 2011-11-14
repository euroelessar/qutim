/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PERSONINFO_H
#define PERSONINFO_H

#include "libqutim_global.h"
#include "localizedstring.h"
#include <QSharedDataPointer>
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class PersonInfoData;

	/**
	* @brief PersonInfo is container of information about person.
	*
	* @code
Encoding is utf-8, language russian
PersonInfo author = PersonInfo(QT_TRANSLATE_NOOP("Author", "Ivan Sidoroff"),
							   QT_TRANSLATE_NOOP("Task", "Plugin author"),
							   "v.sidorov@qutim.org",
							   "http://qutim.org/v.sidorov");
 author.name(); // "Иван Сидоров"
 author.task(); // "Автор плагина"
	@endcode
	*/
	class LIBQUTIM_EXPORT PersonInfo
	{
	public:
		/**
		* @brief Constructor
		*
		* @param name Person's name
		* @param task Person's task
		* @param email Person's e-mail
		* @param web Person's web address
		*/
		PersonInfo(const LocalizedString &name = LocalizedString(),
				   const LocalizedString &task = LocalizedString(),
				   const QString &email = QString(), const QString &web = QString());
		/**
		* @brief Constructor
		*
		* @param ocsUsername The person's Open Collaboration Services username.
		*/
		PersonInfo(const QString &ocsUsername);
		/**
		* @brief Constructs copy of @a other
		*/
		PersonInfo(const PersonInfo &other);
		PersonInfo(const QSharedDataPointer<PersonInfoData> &p);
		/**
		* @brief Destructor
		*/
		~PersonInfo();
		/**
		* @brief Assigns @a other to this info
		*
		* @return Reference to this info
		*/
		PersonInfo &operator =(const PersonInfo &other);
		/**
		* @brief Set person's name to @a name
		*
		* @return Reference to this info
		*/
		PersonInfo &setName(const LocalizedString &name);
		/**
		* @brief Set person's task to @a task
		*
		* @return Reference to this info
		*/
		PersonInfo &setTask(const LocalizedString &task);
		/**
		* @brief Set person's e-mail to @a email
		*
		* @return Reference to this info
		*/
		PersonInfo &setEmail(const QString &email);
		/**
		* @brief Set person's web address to @a web
		*
		* @return Reference to this info
		*/
		PersonInfo &setWeb(const QString &web);
		/**
		* @brief Person's name
		*
		* @return Person's name
		*/
		LocalizedString name() const;
		/**
		* @brief Person's task
		*
		* @return Person's task
		*/
		LocalizedString task() const;
		/**
		* @brief Person's e-mail
		*
		* @return Person's e-mail
		*/
		QString email() const;
		/**
		* @brief Person's web address
		*
		* @return Person's web address
		*/
		QString web() const;
		QString ocsUsername() const;

		static QList<PersonInfo> authors();
		static QList<PersonInfo> translators();
#ifndef Q_QDOC
	private:
		QSharedDataPointer<PersonInfoData> d;
	public:
		typedef PersonInfoData Data;
		Data *data();
#endif
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::PersonInfo)

#endif // PERSONINFO_H

