/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef UREEN_CONNECTIONMANAGER_H
#define UREEN_CONNECTIONMANAGER_H

#include <QObject>
#include <QVariantMap>
#include "pendingreply.h"

namespace Ureen {

typedef QMap<QString, QVariantMap> ProtocolPropertiesMap;

class ConnectionManagerPrivate;

class ConnectionManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ConnectionManager)
    Q_PROPERTY(Ureen::ProtocolPropertiesMap protocols READ protocols CONSTANT)
public:
    enum ParameterFlag {
        Required = 1,
        Register = 2,
        HasDefault = 4,
        Secret = 8,
        Property = 16
    };
    Q_DECLARE_FLAGS(ParameterFlags, ParameterFlag)

    struct ParameterSpecification
    {
        QString name;
        ParameterFlags flags;
        QString signature;
        QVariant defaultValue;
    };

    explicit ConnectionManager(QObject *parent = 0);
    ~ConnectionManager();

    QStringList listProtocols() const;

    ProtocolPropertiesMap protocols() const;
    
signals:
    
public slots:
    virtual PendingReply<QList<ParameterSpecification> > getParameters(const QString &protocol);

protected:
    void updateProtocols(const ProtocolPropertiesMap &protocols);
    
private:
    QScopedPointer<ConnectionManagerPrivate> d_ptr;
};

} // namespace Ureen

#endif // UREEN_CONNECTIONMANAGER_H
