/****************************************************************************
 *  account.h
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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "configbase.h"
#include <QMetaType>

namespace qutim_sdk_0_3
{
	class Contact;
	class Protocol;
	struct AccountPrivate;

	class LIBQUTIM_EXPORT Account : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
		Q_PROPERTY(Status status READ status WRITE setStatus)
	public:
		Account(const QString &id, Protocol *protocol);
		virtual ~Account();
		QString id() const;
		Config config();
		ConfigGroup config(const QString &group);
		Status status() const;
		virtual void setStatus(Status status);
	private:
		QScopedPointer<AccountPrivate> p;
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::Account*)

#endif // ACCOUNT_H
