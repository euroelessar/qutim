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

#include "telepathyaccount.h"

TelepathyAccount::TelepathyAccount(const QDBusConnection &bus,
                                   const QString &busName,
                                   const QString &objectPath,
                                   const Tp::ConnectionFactoryConstPtr &connectionFactory,
                                   const Tp::ChannelFactoryConstPtr &channelFactory,
                                   const Tp::ContactFactoryConstPtr &contactFactory,
                                   const Tp::Feature &coreFeature)
    : Tp::Account(bus, busName, objectPath, connectionFactory,
                  channelFactory, contactFactory, coreFeature),
      m_account(NULL)
{
}

void TelepathyAccount::setAccount(::Account *account)
{
    m_account = account;
}

::Account *TelepathyAccount::account()
{
    return m_account;
}
