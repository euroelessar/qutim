/****************************************************************************
 *  contactlist.h
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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class ChatUnit;
	class Account;

	class LIBQUTIM_EXPORT ContactList : public QObject
	{
		Q_OBJECT
	public:
		static ContactList *instance();
	public slots:
		virtual void addContact(qutim_sdk_0_3::ChatUnit *unit) = 0;
		virtual void removeContact(qutim_sdk_0_3::ChatUnit *unit) = 0;
		virtual void removeAccount(qutim_sdk_0_3::Account *account) = 0;
	protected:
		ContactList();
		virtual ~ContactList();
		virtual void virtual_hook(int id, void *data);
	};
}

#endif // CONTACTLIST_H
