/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "resultmodel.h"
#include <qutim/itemdelegate.h>

namespace Core {

ResultModel::ResultModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void ResultModel::setRequest(const RequestPtr &request)
{
    beginResetModel();
    if (m_request)
        m_request->disconnect(this);
    m_request = request;
    if (m_request) {
        connect(m_request.data(), SIGNAL(rowAboutToBeAdded(int)), SLOT(onRowAboutToBeAdded(int)));
        connect(m_request.data(), SIGNAL(rowAdded(int)), SLOT(onRowAdded(int)));
    }
    endResetModel();
}

int ResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (m_request)
        return m_request->rowCount();
    else
        return 0;
}

int ResultModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (m_request)
        return m_request->columnCount();
    return 0;
}

QVariant ResultModel::data(const QModelIndex &index, int role) const
{
    if (!m_request)
        return QVariant();
    if (role == DescriptionRole) {
        int row = index.row();
        QVariant fieldsVar = m_request->data(row, DescriptionRole);
        if (fieldsVar.isNull()) {
            QVariantMap fields;
            for (int i = 1, c = m_request->columnCount(); i < c; ++i) {
                QVariant data = m_request->data(row, i, Qt::DisplayRole);
                if (data.isNull())
                    continue;
                if (data.canConvert(QVariant::String) && data.toString().isEmpty())
                    continue;
                fields.insert(m_request->headerData(i, Qt::DisplayRole).toString(), data);
            }
            return fields;
        }
        return fieldsVar;
    } else {
        return m_request->data(index.row(), index.column(), role);
    }
}

QVariant ResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (m_request && orientation == Qt::Horizontal)
        return m_request->headerData(section, role);
    return QVariant();
}

void ResultModel::onRowAboutToBeAdded(int row)
{
    beginInsertRows(QModelIndex(), row, row);
}

void ResultModel::onRowAdded(int row)
{
    Q_UNUSED(row);
    endInsertRows();
    emit rowAdded(row);
}


} // namespace Core

