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

#include "libqutim_global.h"
#include <QSet>
#include <QIcon>

namespace qutim_sdk_0_3
{
	class Account;
	class MetaContact;
	class Message;

	class Contact : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(QSet<QString> tags READ tags WRITE setTags NOTIFY tagsChanged)
		Q_PROPERTY(Status status READ status NOTIFY statusChanged)
		Q_PROPERTY(QIcon statusIcon READ statusIcon NOTIFY statusIconChanged)
	public:
		enum Type { Meta, Simple, Sub };
		Contact(Account *account);
		Type type() const { return m_type; }
		virtual QSet<QString> tags() const;
		virtual QString id() const = 0;
		virtual QString name() const;
		virtual Status status() const;
		virtual QIcon statusIcon() const;
		Account *account() { return m_account; }
		const Account *account() const { return m_account; }
		MetaContact *metaContact() { return m_metacontact; }
		const MetaContact *metaContact() const { return m_metacontact; }
		virtual void sendMessage(const Message &message) = 0;
		virtual void setName(const QString &name) = 0;
		virtual void setTags(const QSet<QString> &tags) = 0;
	signals:
		void statusChanged(Status status);
		void statusIconChanged(const QIcon &statusIcon);
		void nameChanged(const QString &name);
		void tagsChanged(const QSet<QString> &tags);
	protected:
		Type m_type;
		Account *m_account;
		MetaContact *m_metacontact;
	};
}

#endif // CONTACT_H
