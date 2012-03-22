/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QRegExp>
#include <QStringList>

#include "mrimprotocol.h"
#include "mrimaccount.h"
#include "utils.h"
#include <qutim/statusactiongenerator.h>

#include "../ui/wizards/mrimaccountwizard.h"

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

void MrimProtocol::loadActions()
{
	QList<Status> statuses;
	statuses << Status(Status::Online)
			 << Status(Status::FreeChat)
			 << Status(Status::Away)
			 << Status(Status::NA)
			 << Status(Status::DND)
			 << Status(Status::Invisible)
			 << Status(Status::Offline);

	Status connectingStatus(Status::Connecting);
	connectingStatus.initIcon("mrim");
	Status::remember(connectingStatus, "mrim");

	foreach (Status status, statuses) {
		status.initIcon("mrim");
		Status::remember(status, "mrim");
		MenuController::addAction<MrimAccount>(new StatusActionGenerator(status));
	}
}

void MrimProtocol::addAccount(MrimAccount *account)
{
	p->m_accountsHash.insert(account->id(), account);
	emit accountCreated(account);
	connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDestroyed(QObject*)));
}

void MrimProtocol::loadAccounts()
{
	loadActions();
    QStringList accounts = config("general").value("accounts",QStringList());
	debug() << Q_FUNC_INFO << accounts;

    foreach (QString email, accounts) {
		addAccount(new MrimAccount(email));
    }
}

QVariant MrimProtocol::data(DataType type)
{
	switch (type) {
		case ProtocolIdName:
			return "E-Mail";
		case ProtocolContainsContacts:
			return true;
		default:
			return QVariant();
	}
}

void MrimProtocol::onAccountDestroyed(QObject *obj)
{
	p->m_accountsHash.remove(p->m_accountsHash.key(static_cast<MrimAccount*>(obj)));
}

MrimProtocol* MrimProtocol::instance()
{
    if (!self)
    {
		warning() << "MrimProtocol isn't created yet!";
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
			addAccount(account);

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

