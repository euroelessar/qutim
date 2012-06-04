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

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "configbase.h"
#include <QStringList>

class QWizardPage;

namespace qutim_sdk_0_3
{
class ExtensionInfo;
class Contact;
class Account;
class ProtocolPrivate;

class Protocol;
typedef QHash<QString, Protocol *> ProtocolHash;

class LIBQUTIM_EXPORT AccountCreationWizard : public QObject
{
    Q_OBJECT
public:
    AccountCreationWizard(Protocol *protocol);
    virtual ~AccountCreationWizard();
    virtual QList<QWizardPage *> createPages(QWidget *parent) = 0;
    ExtensionInfo info() const;
protected:
    void setInfo(const ExtensionInfo &info);
    virtual void virtual_hook(int id, void *data);
};

class LIBQUTIM_EXPORT Protocol : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Protocol)
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QStringList supportedAccountParameters READ supportedAccountParameters CONSTANT)
    //		Q_FLAGS(DataType DataTypes)
    //		Q_FLAGS(RemoveFlag RemoveFlags)
public:
    enum DataType {
        ProtocolIdName,
        ProtocolContainsContacts
    };
    enum RemoveFlag {
        DeleteAccount = 0x01
    };
    enum ProtocolHook {
        SupportedAccountParametersHook,
        CreateAccountHook
    };

    struct CreateAccountArgument
    {
        QString id;
        QVariantMap parameters;
        Account *account;
    };

    //		Q_DECLARE_FLAGS(RemoveFlags, RemoveFlag)
    //		Q_DECLARE_FLAGS(DataTypes, DataType)
    Protocol();
    Protocol(ProtocolPrivate &p);
    virtual ~Protocol();
    Config config();
    ConfigGroup config(const QString &group);
    QString id() const;
    QStringList supportedAccountParameters() const;
    Q_INVOKABLE Account *createAccount(const QString &id, const QVariantMap &parameters);
    Q_INVOKABLE virtual QList<qutim_sdk_0_3::Account*> accounts() const = 0;
    Q_INVOKABLE virtual qutim_sdk_0_3::Account *account(const QString &id) const = 0;
    virtual QVariant data(DataType type);
    /*!
          Remove account from qutIM, protocols can reimplement this method.
          Be careful, this method actually delete your account.
          It is recommended to display a confirmation dialog box.
        */
    Q_INVOKABLE virtual void removeAccount(qutim_sdk_0_3::Account *account, RemoveFlag flags = DeleteAccount);
    static ProtocolHash all();
signals:
    void accountCreated(qutim_sdk_0_3::Account *);
    void accountRemoved(qutim_sdk_0_3::Account *);
protected:
    virtual void virtual_hook(int id, void *data);
private:
    virtual void loadAccounts() = 0;
    friend class ModuleManager;
protected:
    QScopedPointer<ProtocolPrivate> d_ptr;
};
}

#endif // PROTOCOL_H

