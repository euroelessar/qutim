/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

	void doConnectToServer();
	void doDisconnectFromServer();
	void doStatusChange(const Status &status);
};
}

#endif // METACONTACTMANAGER_H

