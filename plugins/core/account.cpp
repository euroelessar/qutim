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
#include "contactmanager_p.h"
#include "functional.h"
#include <TelepathyQt/ConnectionManager>

Account::Account(AccountPrivate &priv, QObject *parent) :
    QObject(parent), d(&priv)
{
    d->contactManager = new ContactManager(this);

    connect(d->account.data(), &Tp::Account::serviceNameChanged,
            this, &Account::serviceNameChanged);
    connect(d->account.data(), &Tp::Account::displayNameChanged,
            this, &Account::displayNameChanged);
    connect(d->account.data(), &Tp::Account::iconNameChanged,
            this, &Account::iconNameChanged);
    connect(d->account.data(), &Tp::Account::nicknameChanged,
            this, &Account::nicknameChanged);
    connect(d->account.data(), &Tp::Account::normalizedNameChanged,
            this, &Account::normalizedNameChanged);

    connect(d->account.data(), &Tp::Account::connectionChanged,
            [this] (const Tp::ConnectionPtr &connection) {
        auto contactManager = connection ? connection->contactManager() : Tp::ContactManagerPtr();
        d->contactManager->d->setContactManager(contactManager);
    });
    if (d->account->connection())
        d->contactManager->d->setContactManager(d->account->connection()->contactManager());

    d->account->becomeReady(Tp::Account::FeatureCore);

//    lconnect(d->account->becomeReady(Tp::Account::FeatureCore),
//             [this] (Tp::PendingReady *) {
//    });
}

Account::~Account()
{
    delete d->contactManager;
    d->contactManager = NULL;
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

QString Account::uniqueIdentifier() const
{
    return d->account->uniqueIdentifier();
}

QString Account::normalizedName() const
{
    return d->account->normalizedName();
}

ContactManager *Account::contactManager() const
{
    return d->contactManager;
}
