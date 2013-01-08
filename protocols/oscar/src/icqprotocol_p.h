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

#ifndef ICQPROTOCOL_P_H
#define ICQPROTOCOL_P_H

#include "icqaccount.h"
#include "icqprotocol.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqProtocolPrivate : public QObject
{
	Q_OBJECT
public:
	inline IcqProtocolPrivate() :
		accounts_hash(new QHash<QString, QPointer<IcqAccount> > ())
	{ }
	inline ~IcqProtocolPrivate() { delete accounts_hash; }
	union
	{
		QHash<QString, QPointer<IcqAccount> > *accounts_hash;
		QHash<QString, IcqAccount *> *accounts;
	};
public slots:
	void removeAccount(QObject *obj);
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQPROTOCOL_P_H

