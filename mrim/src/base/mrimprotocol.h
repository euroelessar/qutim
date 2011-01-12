/****************************************************************************
 *  mrimprotocol.h
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
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

private:
    Q_DISABLE_COPY(MrimProtocol)
	void addAccount(MrimAccount *account);
	void loadActions();
    void loadAccounts();

    QScopedPointer<MrimProtocolPrivate> p;
    static MrimProtocol* self;
};

#endif // MRIMPROTOCOL_H
