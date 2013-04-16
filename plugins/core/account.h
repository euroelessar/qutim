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

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <qqml.h>

class AccountPrivate;

class Account : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString protocolName READ protocolName CONSTANT)
    Q_PROPERTY(QString serviceName READ serviceName NOTIFY serviceNameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString nickname READ nickname NOTIFY nicknameChanged)
public:
    explicit Account(AccountPrivate &priv, QObject *parent);
    ~Account();
    
    QString protocolName() const;
    QString serviceName() const;
    QString displayName() const;
    QString iconName() const;
    QString nickname() const;

signals:
    void serviceNameChanged(const QString &serviceName);
    void displayNameChanged(const QString &displayName);
    void iconNameChanged(const QString &iconName);
    void nicknameChanged(const QString &nickname);

public slots:
    
private:
    QScopedPointer<AccountPrivate> d;
};

QML_DECLARE_TYPE(Account)

#endif // ACCOUNT_H
