/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#include "vprotocol.h"
#include "vcontact.h"
#include "vaccount.h"

#include <qutim/status.h>
#include <qutim/statusactiongenerator.h>
#include <qutim/icon.h>

#include <QDesktopServices>
#include <QUrl>

using namespace qutim_sdk_0_3;

static VProtocol *self = 0;

VProtocol::VProtocol() :
    qutim_sdk_0_3::Protocol()
{
    Q_ASSERT(!self);
    self = this;
}

VProtocol::~VProtocol()
{
}

qutim_sdk_0_3::Account *VProtocol::account(const QString &email) const
{
    return m_accounts.value(email);
}

void VProtocol::loadAccounts()
{
    QList<Status> statuses;
    statuses << Status(Status::Online)
             << Status(Status::Offline)
             << Status(Status::Invisible);
    foreach (Status status, statuses) {
        status.initIcon("vkontakte");
        Status::remember(status, "vkontakte");
        MenuController::addAction(new StatusActionGenerator(status), &VAccount::staticMetaObject);
    }

    ActionGenerator *gen = new ActionGenerator(Icon("applications-internet"),
                                               QT_TRANSLATE_NOOP("Vkontakte", "Open homepage"),
                                               this,
                                               SLOT(onWebPageTriggered(QObject*)));
    gen->setType(ActionTypeContactList);
    MenuController::addAction<VContact>(gen);

    QStringList accounts = config("general").value("accounts", QStringList());
    foreach (const QString &email, accounts) {
        VAccount *account = new VAccount(email, this);
        addAccount(account);
    }
}

QList<qutim_sdk_0_3::Account *> VProtocol::accounts() const
{
    AccountList list;
    foreach (VAccount *account, m_accounts)
        list.append(account);
    return list;
}

QVariant VProtocol::data(qutim_sdk_0_3::Protocol::DataType type)
{
    switch (type) {
    case ProtocolIdName:
        return tr("id");
    case ProtocolContainsContacts:
        return true;
    default:
        return QVariant();
    }
}

VProtocol *VProtocol::instance()
{
    if (!self)
        self = new VProtocol();
    return self;
}

void VProtocol::addAccount(VAccount *account)
{
    m_accounts.insert(account->email(), account);
    account->loadSettings();
    connect(account, SIGNAL(destroyed(QObject*)), this, SLOT(onAccountDestroyed(QObject*)));
    emit accountCreated(account);
}

Account *VProtocol::doCreateAccount(const QString &email, const QVariantMap &parameters)
{
    const QString password = parameters.value("password").toString();
    const bool savePassword = parameters.value("savePassword", false).toBool();

    VAccount *account = new VAccount(email, this);
    if (savePassword) {
        Config cfg = account->config();
        cfg.beginGroup("general");
        cfg.setValue("passwd", password, Config::Crypted);
    }
    Config cfg = config();
    cfg.beginGroup("general");
    QStringList accounts = cfg.value("accounts", QStringList());
    accounts << account->id();
    cfg.setValue("accounts", accounts);
    addAccount(account);
    return account;
}

void VProtocol::virtual_hook(int id, void *data)
{
    switch (id) {
    case SupportedAccountParametersHook: {
        QStringList &properties = *reinterpret_cast<QStringList*>(data);
        properties << QLatin1String("password");
        break;
    }
    case CreateAccountHook: {
        CreateAccountArgument &argument = *reinterpret_cast<CreateAccountArgument*>(data);
        argument.account = doCreateAccount(argument.id, argument.parameters);
        break;
    }
    default:
        Protocol::virtual_hook(id, data);
    }
}

void VProtocol::onWebPageTriggered(QObject *obj)
{
    VContact *contact = qobject_cast<VContact*>(obj);
    Q_ASSERT(obj);
    QUrl url ("http://vkontakte.ru/id" + contact->id());
    QDesktopServices::openUrl(url);
}

void VProtocol::onAccountDestroyed(QObject *obj)
{
    VAccount *account = static_cast<VAccount*>(obj);
    m_accounts.remove(m_accounts.key(account));
}
