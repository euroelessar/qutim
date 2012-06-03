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

#ifndef ICQPROTOCOL_H
#define ICQPROTOCOL_H

#include <qutim/protocol.h>
#include <qutim/status.h>
#include "icq_global.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqProtocolPrivate;
class IcqProtocol;
class IcqAccount;

class LIBOSCAR_EXPORT IcqProtocol: public Protocol
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(IcqProtocol)
	Q_CLASSINFO("Protocol", "icq")
public:
	IcqProtocol();
	virtual ~IcqProtocol();
	static inline IcqProtocol *instance() { if (!self) warning() << "IcqProtocol isn't created"; return self; }
	virtual QList<Account *> accounts() const;
	virtual Account *account(const QString &id) const;
	QHash<QString, IcqAccount *> accountsHash() const;
	void addAccount(IcqAccount *account);
	virtual QVariant data(DataType type);
signals:
	void settingsUpdated();
public slots:
	void updateSettings();
protected:
	void loadAccounts();
	virtual void virtual_hook(int id, void *data);
private:
	QScopedPointer<IcqProtocolPrivate> d_ptr;
	static IcqProtocol *self;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQPROTOCOL_H

