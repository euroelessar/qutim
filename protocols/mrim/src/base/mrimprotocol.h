/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
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


#ifndef MRIMPROTOCOL_H
#define MRIMPROTOCOL_H

#include <qutim/protocol.h>

using namespace qutim_sdk_0_3;

struct MrimProtocolPrivate;
class MrimAccount;

class MrimProtocol : public Protocol
{
    Q_OBJECT
    Q_CLASSINFO("Protocol", "mrim")

public:
    enum AccountCreationError
    {
        None,
        AlreadyExists,
        InvalidArguments
    };

public:
    static MrimProtocol* instance();
    MrimProtocol();
    virtual ~MrimProtocol();
    QList<Account *> accounts() const;
    Account *account(const QString &id) const;
    AccountCreationError createAccount(const QString& email, const QString& password);
	QVariant data(DataType type);
protected slots:
	void onAccountDestroyed(QObject *obj);
private:
    Q_DISABLE_COPY(MrimProtocol)
	void addAccount(MrimAccount *account);
	void loadActions();
    void loadAccounts();

    QScopedPointer<MrimProtocolPrivate> p;
    static MrimProtocol* self;
};

#endif // MRIMPROTOCOL_H

