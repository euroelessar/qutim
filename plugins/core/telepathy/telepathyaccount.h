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

#ifndef TELEPATHYACCOUNT_H
#define TELEPATHYACCOUNT_H

#include <TelepathyQt/Account>
#include "../account_p.h"

class TelepathyAccount : public Tp::Account
{
    Q_OBJECT
public:
    typedef Tp::SharedPtr<TelepathyAccount> Ptr;

    explicit TelepathyAccount(const QDBusConnection &bus,
                              const QString &busName, const QString &objectPath,
                              const Tp::ConnectionFactoryConstPtr &connectionFactory,
                              const Tp::ChannelFactoryConstPtr &channelFactory,
                              const Tp::ContactFactoryConstPtr &contactFactory,
                              const Tp::Feature &coreFeature);

    void setAccount(::Account *account);
    ::Account *account();

private:
    ::Account *m_account;
};

#endif // TELEPATHYACCOUNT_H
