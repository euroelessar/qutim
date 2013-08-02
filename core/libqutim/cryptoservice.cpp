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

#include "cryptoservice.h"

#ifdef INSIDE_MODULE_MANAGER
#undef INSIDE_MODULE_MANAGER

#include "modulemanager.h"
#include <QDataStream>
#include <QBuffer>
#include <QCoreApplication>
#include "debug.h"

namespace qutim_sdk_0_3
{
// is must be named as a lot of another variables
static QPointer<CryptoService> self;

CryptoService::CryptoService()
{
    if (self) {
        qFatal("Double initialization of CryptoService, aborting");
        // ensure
        qApp->exit(1); //goodbye cruel world
    }
    self = this;
}

CryptoService::~CryptoService()
{
    self.clear();
}

QVariant CryptoService::crypt(const QVariant &value)
{
    return self.isNull() ? value : self.data()->cryptImpl(value);
}

QVariant CryptoService::decrypt(const QVariant &value)
{
    return self.isNull() ? value : self.data()->decryptImpl(value);
}

QVariant CryptoService::variantFromData(const QByteArray &data) const
{
    QVariant result;
    QByteArray tmp = data;
    {
        QBuffer buffer(&tmp);
        buffer.open(QIODevice::ReadOnly);
        QDataStream stream(&buffer);
        stream.setVersion(QDataStream::Qt_4_5);
        quint32 type;
        stream >> type;
        if(type == QVariant::String)
        {
            QByteArray tmp2;
            stream >> tmp2;
            result = QString::fromUtf8(tmp2, tmp2.size());
        }
        else
        {
            buffer.seek(0);
            result.load(stream);
        }
    }
    return result;
}

QByteArray CryptoService::dataFromVariant(const QVariant &value) const
{
    QByteArray result;
    {
        QDataStream stream(&result, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_5);
        if(value.type() == QVariant::String)
        {
            stream << quint32(QVariant::String);
            stream << value.toString().toUtf8();
        }
        else
            value.save(stream);
    }
    return result;
}
}

#endif // INSIDE_MODULE_MANAGER

