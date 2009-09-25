/****************************************************************************
 *  protocol.h
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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "configbase.h"

namespace qutim_sdk_0_3
{
	class Contact;
	class Account;
	struct ProtocolPrivate;

	class Protocol : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QString id READ id)
	public:
		Protocol();
		virtual ~Protocol();
		Config config();
		ConfigGroup config(const QString &group);
		QString id();
	private:
		virtual void loadAccounts() = 0;
		friend class ModuleManager;
		QScopedPointer<ProtocolPrivate> p;
	};
}

#endif // PROTOCOL_H
