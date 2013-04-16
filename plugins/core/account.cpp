/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "account_p.h"

Account::Account(AccountPrivate &priv, QObject *parent) :
    QObject(parent), d(&priv)
{
    connect(d->account.data(), &Tp::Account::serviceNameChanged,
            this, &Account::serviceNameChanged);
    connect(d->account.data(), &Tp::Account::displayNameChanged,
            this, &Account::displayNameChanged);
    connect(d->account.data(), &Tp::Account::iconNameChanged,
            this, &Account::iconNameChanged);
    connect(d->account.data(), &Tp::Account::nicknameChanged,
            this, &Account::nicknameChanged);
}

Account::~Account()
{
}

QString Account::protocolName() const
{
    return d->account->protocolName();
}

QString Account::serviceName() const
{
    return d->account->serviceName();
}

QString Account::displayName() const
{
    return d->account->displayName();
}

QString Account::iconName() const
{
    return d->account->iconName();
}

QString Account::nickname() const
{
    return d->account->nickname();
}
