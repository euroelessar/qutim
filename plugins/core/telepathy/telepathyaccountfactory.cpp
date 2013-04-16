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

#include "telepathyaccountfactory.h"
#include "telepathyaccount.h"
#include "../account_p.h"

TelepathyAccountFactory::TelepathyAccountFactory(const QDBusConnection &bus,
                                                 const Tp::Features &features)
    : Tp::AccountFactory(bus, features)
{
}

Tp::AccountFactoryPtr TelepathyAccountFactory::create(const QDBusConnection &bus,
                                                      const Tp::Features &features)
{
    return Tp::AccountFactoryPtr(new TelepathyAccountFactory(bus, features));
}

Tp::AccountPtr TelepathyAccountFactory::construct(
        const QString &busName,
        const QString &objectPath,
        const Tp::ConnectionFactoryConstPtr &connFactory,
        const Tp::ChannelFactoryConstPtr &chanFactory,
        const Tp::ContactFactoryConstPtr &contactFactory) const
{
    auto account = Tp::AccountPtr(new TelepathyAccount(dbusConnection(), busName, objectPath,
                                                       connFactory, chanFactory, contactFactory,
                                                       Tp::Account::FeatureCore));
    auto priv = new AccountPrivate;
    priv->account = account;
    Tp::SharedPtr<TelepathyAccount>::staticCast(account)->setAccount(
                new Account(*priv, const_cast<TelepathyAccountFactory *>(this)));
    return account;
}
