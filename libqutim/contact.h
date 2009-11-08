/****************************************************************************
 *  contact.h
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

#ifndef CONTACT_H
#define CONTACT_H

#include "chatunit.h"
#include <QSet>
#include <QIcon>
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class Account;
	class MetaContact;
	class Message;

	class LIBQUTIM_EXPORT Contact : public ChatUnit
	{
		Q_OBJECT
		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(QSet<QString> tags READ tags WRITE setTags NOTIFY tagsChanged)
		Q_PROPERTY(Status status READ status NOTIFY statusChanged)
		Q_PROPERTY(QIcon statusIcon READ statusIcon NOTIFY statusIconChanged)
		Q_PROPERTY(bool inList READ isInList WRITE setInList NOTIFY inListChanged)
	public:
		Contact(Account *account);
		virtual ~Contact();
		virtual QSet<QString> tags() const;
		virtual QString name() const;
		virtual Status status() const;
		virtual QIcon statusIcon() const;
		virtual void sendMessage(const Message &message) = 0;
		virtual void setName(const QString &name) = 0;
		virtual void setTags(const QSet<QString> &tags) = 0;
		virtual bool isInList() const = 0;
		virtual void setInList(bool inList) = 0;
		void addToList() { setInList(true); }
		void removeFromList() { setInList(false); }
	signals:
		void statusChanged(Status status);
		void statusIconChanged(const QIcon &statusIcon);
		void nameChanged(const QString &name);
		void tagsChanged(const QSet<QString> &tags);
		void inListChanged(bool inList);
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Contact*)

#endif // CONTACT_H
