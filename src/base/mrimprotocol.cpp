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

#include <QRegExp>
#include <QStringList>

#include "mrimprotocol.h"
#include "mrimaccount.h"
#include "utils.h"

#include "ui/wizards/mrimaccountwizard.h"

MrimProtocol* MrimProtocol::self = 0;

struct MrimProtocolPrivate
{
    QHash<QString,MrimAccount*> m_accountsHash;
};

MrimProtocol::MrimProtocol() : p(new MrimProtocolPrivate())
{
    Q_ASSERT(!self);
    self = this;
}

MrimProtocol::~MrimProtocol()
{
}

QList<Account *> MrimProtocol::accounts() const
{
    QList<Account *> accounts;
    QHash<QString, MrimAccount* >::const_iterator it;

    for (it = p->m_accountsHash.begin(); it != p->m_accountsHash.end(); it++)
    {
        accounts.append(it.value());
    }
    return accounts;
}

Account *MrimProtocol::account(const QString &id) const
{
    return p->m_accountsHash.value(id);
}

void MrimProtocol::loadAccounts()
{
    QStringList accounts = config("general").value("accounts",QStringList());

    foreach (QString email, accounts)
    {
        p->m_accountsHash.insert(email,new MrimAccount(email));
    }
}

MrimProtocol* MrimProtocol::instance()
{
    if (!self)
    {
        qWarning("MrimProtocol isn't created yet!");
    }
    return self;
}

MrimProtocol::AccountCreationError MrimProtocol::createAccount(const QString& email, const QString& password)
{
    AccountCreationError err = None;

    QString validEmail = Utils::stripEmail(email);

    if (!validEmail.isEmpty())
    {//email is compliant
        ConfigGroup cfg = config("general");
        QStringList accounts = cfg.value("accounts",QStringList());

        if (!accounts.contains(validEmail))
        {//account is new, saving
            MrimAccount *account = new MrimAccount(validEmail);
            account->config().group("general").setValue("passwd", password, Config::Crypted);
            account->config().sync();//save account settings
            p->m_accountsHash.insert(validEmail,account);

            accounts << validEmail;
            cfg.setValue("accounts",accounts);
            cfg.sync(); //save global settings
        }
        else
        {
            err = AlreadyExists;
        }
    }
    else
    {
        err = InvalidArguments;
    }
    return err;
}
