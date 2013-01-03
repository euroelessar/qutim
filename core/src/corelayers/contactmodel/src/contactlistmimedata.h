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

#ifndef CONTACTLISTMIMEDATA_H
#define CONTACTLISTMIMEDATA_H

#include <qutim/mimeobjectdata.h>
#include <QModelIndex>

class ContactListMimeData : public qutim_sdk_0_3::MimeObjectData
{
    Q_OBJECT
public:
    explicit ContactListMimeData();
    virtual ~ContactListMimeData();

    void setIndexes(const QModelIndexList &indexes);
    void setIndexes(const QList<QPersistentModelIndex> &indexes);
    QList<QPersistentModelIndex> indexes() const;
    static QString modelIndexListMimeType();

    virtual bool hasFormat(const QString &mimetype) const;
    virtual QStringList formats() const;

private:
    QList<QPersistentModelIndex> m_indexes;
};

#endif // CONTACTLISTMIMEDATA_H
