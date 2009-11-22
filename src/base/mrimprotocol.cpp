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

AccountCreationWizard *MrimProtocol::accountCreationWizard()
{
    return new MrimAccountWizard(this);
}

void MrimProtocol::loadAccounts()
{
    QStringList accounts = config("accounts").value("list",QStringList());

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

    //Verify email by RFC 2822 spec with additionaly verified domain
    QRegExp emailRx("(\\b[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*\\@(?:mail.ru|bk.ru|inbox.ru|list.ru|corp.mail.ru)\\b)");
    int matchIndex = emailRx.indexIn(email);

    if (matchIndex >= 0)
    {//email is compliant
        QString validEmail = emailRx.cap(1);
        QStringList accounts = config("accounts").value("list",QStringList());

        if (!accounts.contains(validEmail))
        {//account is new, saving
            accounts << validEmail;
            config("accounts").setValue("list",accounts);
            config("accounts").group("validEmail").setValue("password",password,ConfigBase::Crypted);
            p->m_accountsHash.insert(validEmail,new MrimAccount(validEmail));
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
