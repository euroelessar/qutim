/****************************************************************************
 *  abstractcontact.h
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

#ifndef ABSTRACTCONTACT_H
#define ABSTRACTCONTACT_H

#include "chatunit.h"
#include "status.h"
#include <QIcon>
#include <QScopedPointer>

#ifndef Q_QDOC
namespace qutim_sdk_0_3
{
	struct AbstractContactPrivate;
	class MetaContact;
	class Message;

    class LIBQUTIM_EXPORT AbstractContact : public ChatUnit
	{
		Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
		Q_PROPERTY(Status status READ status NOTIFY statusChanged)
	public:
        AbstractContact(const QString &id, Account *parent = 0);
		virtual ~AbstractContact();
		QString id();
		virtual QString name() const;
		virtual Status status() const;
		MetaContact *metaContact();
		virtual void sendMessage(const Message &message) = 0;
		virtual void setName(const QString &name) = 0;
	signals:
		void statusChanged(const Status &status);
		void nameChanged(const QString &name);
	private:
		QScopedPointer<AbstractContactPrivate> p;
	};
}
#endif

#endif // ABSTRACTCONTACT_H
