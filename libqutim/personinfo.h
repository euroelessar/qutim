/****************************************************************************
 *  personinfo.h
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
	* Encoding is utf-8, language russian
	* PersonInfo author = PersonInfo(QT_TRANSLATE_NOOP("Author", "Ivan Sidoroff"),
	*								 QT_TRANSLATE_NOOP("Task", "Plugin author"),
	*								 "v.sidorov@qutim.org",
	*								 "http://qutim.org/v.sidorov");
	* author.name(); // "Иван Сидоров"
	* author.task(); // "Автор плагина"
	* @endcode
	*/
	class LIBQUTIM_EXPORT PersonInfo
	{
	public:
		/**
		* @brief Contructor
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
		* @brief Constructs copy of other
		* @param other What to copy
		*/
		PersonInfo(const PersonInfo &other);
		/**
		* @brief Destructor
		*/
		~PersonInfo();
		/**
		* @brief Assigns @i other to this info
		*
		* @param Info which should be assigned to this one
		* @return Reference to this info
		*/
		PersonInfo &operator =(const PersonInfo &other);
		/**
		* @brief Set person's name
		*
		* @param name Person's name
		* @return Reference to this info
		*/
		PersonInfo &setName(const LocalizedString &name);
		/**
		* @brief Set person's task
		*
		* @param task Person's task
		* @return Reference to this info
		*/
		PersonInfo &setTask(const LocalizedString &task);
		/**
		* @brief Set person's e-mail
		*
		* @param email Person's e-mail
		* @return Reference to this info
		*/
		PersonInfo &setEmail(const QString &email);
		/**
		* @brief Set person's web address
		*
		* @param web Person's web address
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
		const QString &email() const;
		/**
		* @brief Person's web address
		*
		* @return Person's web address
		*/
		const QString &web() const;
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
