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

#include "connectionmanager.h"

namespace Ureen {

class ConnectionManagerPrivate
{
public:
    ProtocolPropertiesMap protocols;
};

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ConnectionManagerPrivate)
{
}

ConnectionManager::~ConnectionManager()
{
}

QStringList ConnectionManager::listProtocols() const
{
    return protocols().keys();
}

ProtocolPropertiesMap ConnectionManager::protocols() const
{
    return d_func()->protocols;
}

PendingReply<QList<ConnectionManager::ParameterSpecification> > ConnectionManager::getParameters(const QString &protocol)
{
    Q_UNUSED(protocol);
    PendingReply<QList<ConnectionManager::ParameterSpecification> > error;
//    error.setError(PendingReply::NotImplemented);
    return error;
}

void ConnectionManager::updateProtocols(const ProtocolPropertiesMap &protocols)
{
    d_func()->protocols = protocols;
}

} // namespace Ureen
