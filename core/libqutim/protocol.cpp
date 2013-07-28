/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "protocol_p.h"
#include "account.h"
#include "contact.h"
#include "extensioninfo.h"
#include "metaobjectbuilder.h"
#include "modulemanager_p.h"

namespace qutim_sdk_0_3
{
AccountCreationWizard::AccountCreationWizard(Protocol *protocol) : QObject(protocol)
{
}

AccountCreationWizard::~AccountCreationWizard()
{
}

ExtensionInfo AccountCreationWizard::info() const
{
    QVariant info = property("protocolinfo");
    if (!info.canConvert<ExtensionInfo>())
        info = parent()->property("extensioninfo");
    return info.value<ExtensionInfo>();
}

void AccountCreationWizard::setInfo(const ExtensionInfo &info)
{
    setProperty("protocolinfo", QVariant::fromValue(info));
}

void AccountCreationWizard::virtual_hook(int id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
}

Protocol::Protocol() : d_ptr(new ProtocolPrivate)
{
}

Protocol::Protocol(ProtocolPrivate &p) : d_ptr(&p)
{
}

Protocol::~Protocol()
{
}

Config Protocol::config()
{
    return Config(id());
}

ConfigGroup Protocol::config(const QString &group)
{
    return config().group(group);
}

QString Protocol::id() const
{
    Q_D(const Protocol);
    if(d->id.isNull())
        d->id = QString::fromUtf8(MetaObjectBuilder::info(metaObject(), "Protocol"));
    return d->id;
}

QStringList Protocol::supportedAccountParameters() const
{
    QStringList properties;
    const_cast<Protocol*>(this)->virtual_hook(SupportedAccountParametersHook, &properties);
    return properties;
}

Account *Protocol::createAccount(const QString &id, const QVariantMap &parameters)
{
    CreateAccountArgument argument = { id, parameters, NULL };
    const_cast<Protocol*>(this)->virtual_hook(CreateAccountHook, &argument);
    return argument.account;
}

void Protocol::virtual_hook(int id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
}

QVariant Protocol::data(DataType type)
{
    switch (type) {
    case ProtocolIdName:
        return "ID";
    case ProtocolContainsContacts:
        return false;
    default:
        return QVariant();
    }
}

void Protocol::removeAccount(Account *account, RemoveFlag flags)
{
    Config general = config().group("general");
    QStringList accounts = general.value("accounts",QStringList());
    accounts.removeAll(account->id());
    general.setValue("accounts",accounts);
    general.sync();
    emit accountRemoved(account);

    if (flags & DeleteAccount)
        account->deleteLater();
}

ProtocolHash Protocol::all()
{
    return allProtocols();
}
}

