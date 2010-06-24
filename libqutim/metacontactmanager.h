/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef METACONTACTMANAGER_H
#define METACONTACTMANAGER_H

#include "account.h"

namespace qutim_sdk_0_3
{
	class ModuleManager;
	class MetaContact;
	class MetaContactManagerPrivate;
	
	class LIBQUTIM_EXPORT MetaContactManager : public Account
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(MetaContactManager)
		Q_CLASSINFO("Service", "MetaContactManager")
	public:
		MetaContactManager();
		~MetaContactManager();
		
		// Create MetaContact with unique ID
		MetaContact *createContact();
		
		static MetaContactManager *instance();
		
		bool eventFilter(QObject *obj, QEvent *ev);
	protected:
		virtual void loadContacts() = 0;
		friend class ModuleManager;
	};
}

#endif // METACONTACTMANAGER_H
