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

#include "contactlistmimedata.h"
#include <QStringList>

using namespace qutim_sdk_0_3;

ContactListMimeData::ContactListMimeData()
{
}

ContactListMimeData::~ContactListMimeData()
{
}


void ContactListMimeData::setIndexes(const QModelIndexList &indexes)
{
    QList<QPersistentModelIndex> persistentIndexes;
    foreach (const QModelIndex &index, indexes)
        persistentIndexes << QPersistentModelIndex(index);
    m_indexes = persistentIndexes;
}

void ContactListMimeData::setIndexes(const QList<QPersistentModelIndex> &indexes)
{
    m_indexes = indexes;
}

QList<QPersistentModelIndex> ContactListMimeData::indexes() const
{
    return m_indexes;
}

QString ContactListMimeData::modelIndexListMimeType()
{
    return QLatin1String("application/qutim-model-index-list");
}

bool ContactListMimeData::hasFormat(const QString &mimetype) const
{
    if (mimetype == QLatin1String("application/qutim-model-index-list"))
        return !m_indexes.isEmpty();
    return MimeObjectData::hasFormat(mimetype);
}

QStringList ContactListMimeData::formats() const
{
    return MimeObjectData::formats()
            << QLatin1String("application/qutim-model-index-list");
}
