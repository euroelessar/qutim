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

#ifndef TELEPATHYACCOUNTFACTORY_H
#define TELEPATHYACCOUNTFACTORY_H

#include <TelepathyQt/AccountFactory>

class TelepathyAccountFactory : public Tp::AccountFactory
{
    Q_OBJECT
public:
    explicit TelepathyAccountFactory(const QDBusConnection &bus, const Tp::Features &features);

    static Tp::AccountFactoryPtr create(const QDBusConnection &bus,
            const Tp::Features &features = Tp::Features());

    virtual Tp::AccountPtr construct(const QString &busName, const QString &objectPath,
            const Tp::ConnectionFactoryConstPtr &connFactory,
            const Tp::ChannelFactoryConstPtr &chanFactory,
            const Tp::ContactFactoryConstPtr &contactFactory) const;
};

#endif // TELEPATHYACCOUNTFACTORY_H
