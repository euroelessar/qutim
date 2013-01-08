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

#include "mimeobjectdata.h"
#include <QStringList>
#include <QPointer>

namespace qutim_sdk_0_3
{
class MimeObjectDataPrivate
{
public:
    QPointer<QObject> object;
};

MimeObjectData::MimeObjectData() : d_ptr(new MimeObjectDataPrivate)
{
}

MimeObjectData::~MimeObjectData()
{
}

void MimeObjectData::setObject(QObject *obj)
{
    d_func()->object = obj;
}

QObject *MimeObjectData::object() const
{
    return d_func()->object.data();
}

QString MimeObjectData::objectMimeType()
{
    return QLatin1String("application/qutim-object");
}

bool MimeObjectData::hasFormat(const QString &mimetype) const
{
    if (mimetype == QLatin1String("application/qutim-object"))
        return !d_func()->object.isNull();
    return QMimeData::hasFormat(mimetype);
}

QStringList MimeObjectData::formats() const
{
    QStringList list = QMimeData::formats();
    if (d_func()->object)
        list << QLatin1String("application/qutim-object");
    return list;
}

QVariant MimeObjectData::retrieveData(const QString &mimetype,
                                      QVariant::Type preferredType) const
{
    return QMimeData::retrieveData(mimetype, preferredType);
}
}

