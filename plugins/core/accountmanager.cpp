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

#include "accountmanager.h"
#include "functional.h"
#include "telepathy/telepathyaccount.h"
#include "telepathy/telepathyaccountfactory.h"
#include <TelepathyQt/AccountManager>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/AccountSet>
#include <TelepathyQt/AccountFilter>
#include <QDBusConnection>

static QWeakPointer<AccountManagerPrivate> self;

class AccountManagerPrivate
{
public:
    typedef void (AccountManager::*ListSignal)(const QList<Account*> &);

    AccountManagerPrivate()
    {
    }

    static Account *convert(const Tp::AccountPtr &account)
    {
        return Tp::SharedPtr<TelepathyAccount>::staticCast(account)->account();
    }

    class AccountAntiFilter : public Tp::AccountFilter
    {
    public:
        AccountAntiFilter() {}
        ~AccountAntiFilter() {}

        bool isValid() const { return true; }
        bool matches(const Tp::AccountPtr &) const { return true; }
    };

    static QSharedPointer<AccountManagerPrivate> create(AccountManager *newManager)
    {
        auto d = self.toStrongRef();
        if (!d) {
            d.reset(new AccountManagerPrivate);
            self = d;

            Tp::AccountFactoryConstPtr accountFactory(new TelepathyAccountFactory(
                                                          QDBusConnection::sessionBus(),
                                                          Tp::Account::FeatureCore));
            auto connectionFactory = Tp::ConnectionFactory::create(
                        QDBusConnection::sessionBus(),
                        Tp::Connection::FeatureConnected
                        | Tp::Connection::FeatureRoster
                        | Tp::Connection::FeatureSimplePresence
                        | Tp::Connection::FeatureRosterGroups);
            auto channelFactory = Tp::ChannelFactory::create(
                        QDBusConnection::sessionBus());
            auto contactFactory = Tp::ContactFactory::create(
                        Tp::Contact::FeatureAlias
                        | Tp::Contact::FeatureAvatarData
                        | Tp::Contact::FeatureSimplePresence);
            d->tpManager = Tp::AccountManager::create(accountFactory,
                                                      connectionFactory,
                                                      channelFactory,
                                                      contactFactory);
            lconnect(d->tpManager->becomeReady(), [] (Tp::PendingReady *) {
                QSharedPointer<AccountManagerPrivate> d = self.toStrongRef();

                auto antiFilter = Tp::AccountFilterConstPtr(new AccountAntiFilter());

                d->addAccountsSet(d->tpManager->filterAccounts(antiFilter),
                                  d->allAccounts,
                                  &AccountManager::allAccountsChanged);
                d->addAccountsSet(d->tpManager->enabledAccounts(),
                                  d->enabledAccounts,
                                  &AccountManager::enabledAccountsChanged);
            });
        }
        d->managers << newManager;

        return d;
    }

    void addAccountsSet(const Tp::AccountSetPtr &accountsSet,
                        QList<Account*> &accountsList,
                        ListSignal changed)
    {
        accountsSets << accountsSet;

        accountsList.clear();

        for (Tp::AccountPtr account : accountsSet->accounts())
            accountsList << convert(account);

        QObject::connect(accountsSet.data(), &Tp::AccountSet::accountAdded,
                [this, &accountsList, changed] (const Tp::AccountPtr &account) {
            accountsList.append(convert(account));
            updateList(accountsList, changed);
        });
        QObject::connect(accountsSet.data(), &Tp::AccountSet::accountRemoved,
                [this, &accountsList, changed] (const Tp::AccountPtr &account) {
            accountsList.removeOne(convert(account));
            updateList(accountsList, changed);
        });

        updateList(accountsList, changed);
    }

    void updateList(const QList<Account*> &accountsList, ListSignal changed)
    {
        for (auto manager : managers)
            (*manager.*changed)(accountsList);
    }

    Tp::AccountManagerPtr tpManager;
    QList<AccountManager*> managers;
    QList<Account*> enabledAccounts;
    QList<Account*> allAccounts;
    QList<Tp::AccountSetPtr> accountsSets;
    QHash<Tp::AccountPtr, Account *> accountsHash;
};

AccountManager::AccountManager(QObject *parent):
    QObject(parent), d(AccountManagerPrivate::create(this))
{
}

AccountManager::~AccountManager()
{
    d->managers.removeOne(this);
}

QList<Account*> AccountManager::enabledAccounts() const
{
    return d->enabledAccounts;
}

QList<Account*> AccountManager::allAccounts() const
{
    return d->allAccounts;
}

