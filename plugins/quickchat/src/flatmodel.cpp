/* This file is part of the KDE project
  Copyright (C) 2010, 2012 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "flatmodel.h"
#include <qutim/debug.h>

#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QItemSelection>

namespace QuickChat
{


FlatProxyModel::FlatProxyModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

void FlatProxyModel::onSourceModelDestroyed()
{
    m_sourceIndexList.clear();
}

void FlatProxyModel::onSourceDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right)
{
    emit dataChanged(mapFromSource(source_top_left), mapFromSource(source_bottom_right));
}

void FlatProxyModel::onSourceHeaderDataChanged(Qt::Orientation orientation, int start, int end)
{
    emit headerDataChanged(orientation, start, end);
}

void FlatProxyModel::onSourceReset()
{
    beginResetModel();
    initiateMaps();
    endResetModel();
}

void FlatProxyModel::onSourceLayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void FlatProxyModel::onSourceLayoutChanged()
{
    initiateMaps();
    emit layoutChanged();
}

void FlatProxyModel::onSourceRowsAboutToBeInserted(const QModelIndex &source_parent, int start, int end)
{
    int rc = sourceModel()->rowCount(source_parent);
    if (rc == 0) {
        Q_ASSERT(start == 0);
        QModelIndex idx = mapFromSource(source_parent);
        //qDebug()<<"start<rc"<<source_parent<<start<<end<<":"<<idx;
        if (idx.isValid()) {
            beginInsertRows(QModelIndex(), idx.row() + start, idx.row() + end);
        } else {
            beginInsertRows(QModelIndex(), start, end);
        }
    } else if (start < rc) {
        QModelIndex source_idx = sourceModel()->index(start, 0, source_parent);
        QModelIndex idx = mapFromSource(source_idx);
        //qDebug()<<"start<rc"<<source_parent<<start<<end<<":"<<idx;
        beginInsertRows(QModelIndex(), idx.row(), idx.row() + end - start);
    } else if (start == rc) {
        QModelIndex source_idx = sourceModel()->index(start - 1, 0, source_parent);
        QModelIndex idx = mapFromSource(source_idx);
        //qDebug()<<"start==rc"<<source_parent<<start<<end<<":"<<idx;
        beginInsertRows(QModelIndex(), idx.row() + 1, idx.row() + 1 + end - start);
    } else {
        qCritical() << "Strange data from source model"<<source_parent<<start<<end;
    }
}

void FlatProxyModel::onSourceRowsInserted(const QModelIndex &source_parent, int start, int end)
{
    Q_UNUSED(source_parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    initiateMaps();
    endInsertRows();
}

void FlatProxyModel::onSourceRowsAboutToBeRemoved(const QModelIndex &source_parent, int start, int end)
{
    Q_UNUSED(source_parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    beginResetModel();
}

void FlatProxyModel::onSourceRowsRemoved(const QModelIndex &source_parent, int start, int end)
{
    Q_UNUSED(source_parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    initiateMaps();
    endResetModel();
}

void FlatProxyModel::onSourceRowsAboutToBeMoved(const QModelIndex &source_parent, int start, int end, const QModelIndex &destParent, int destStart)
{
    beginMoveRows(source_parent, start, end, destParent, destStart);
}

void FlatProxyModel::onSourceRowsMoved(const QModelIndex &source_parent, int start, int end, const QModelIndex &destParent, int destStart)
{
    Q_UNUSED(source_parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destParent);
    Q_UNUSED(destStart);
    initiateMaps();
    endMoveRows();
}

void FlatProxyModel::setSourceModel(QAbstractItemModel *model)
{
    if (sourceModel()) {
        disconnect(sourceModel(), &QAbstractItemModel::dataChanged,
                this, &FlatProxyModel::onSourceDataChanged);

        disconnect(sourceModel(), &QAbstractItemModel::headerDataChanged,
                this, &FlatProxyModel::onSourceHeaderDataChanged);

        disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted,
                this, &FlatProxyModel::onSourceRowsAboutToBeInserted);

        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted,
                this, &FlatProxyModel::onSourceRowsInserted);

        disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved,
                this, &FlatProxyModel::onSourceRowsAboutToBeRemoved);

        disconnect(sourceModel(), &QAbstractItemModel::rowsRemoved,
                this, &FlatProxyModel::onSourceRowsRemoved);

        disconnect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged,
                this, &FlatProxyModel::onSourceLayoutAboutToBeChanged);

        disconnect(sourceModel(), &QAbstractItemModel::layoutChanged,
                this, &FlatProxyModel::onSourceLayoutChanged);

        disconnect(sourceModel(), &QAbstractItemModel::modelReset,
                this, &FlatProxyModel::onSourceReset);

        disconnect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved,
                this, &FlatProxyModel::onSourceRowsAboutToBeMoved);
        disconnect(sourceModel(), &QAbstractItemModel::rowsMoved,
                this, &FlatProxyModel::onSourceRowsMoved);
    }
    QAbstractProxyModel::setSourceModel(model);
    connect(sourceModel(), &QAbstractItemModel::dataChanged,
            this, &FlatProxyModel::onSourceDataChanged);

    connect(sourceModel(), &QAbstractItemModel::headerDataChanged,
            this, &FlatProxyModel::onSourceHeaderDataChanged);

    connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeInserted,
            this, &FlatProxyModel::onSourceRowsAboutToBeInserted);

    connect(sourceModel(), &QAbstractItemModel::rowsInserted,
            this, &FlatProxyModel::onSourceRowsInserted);

    connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &FlatProxyModel::onSourceRowsAboutToBeRemoved);

    connect(sourceModel(), &QAbstractItemModel::rowsRemoved,
            this, &FlatProxyModel::onSourceRowsRemoved);

    connect(sourceModel(), &QAbstractItemModel::layoutAboutToBeChanged,
            this, &FlatProxyModel::onSourceLayoutAboutToBeChanged);

    connect(sourceModel(), &QAbstractItemModel::layoutChanged,
            this, &FlatProxyModel::onSourceLayoutChanged);

    connect(sourceModel(), &QAbstractItemModel::modelReset,
            this, &FlatProxyModel::onSourceReset);

    connect(sourceModel(), &QAbstractItemModel::rowsAboutToBeMoved,
            this, &FlatProxyModel::onSourceRowsAboutToBeMoved);
    connect(sourceModel(), &QAbstractItemModel::rowsMoved,
            this, &FlatProxyModel::onSourceRowsMoved);

    beginResetModel();
    initiateMaps();
    endResetModel();
}

QModelIndex FlatProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex FlatProxyModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int FlatProxyModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_sourceIndexList.count();
}

int FlatProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (sourceModel() == 0) {
        return 0;
    }
    return sourceModel()->columnCount() + 1;
}

bool FlatProxyModel::hasChildren(const QModelIndex &parent) const
{
    return rowCount(parent) > 0;
}

QVariant FlatProxyModel::data(const QModelIndex &index, int role) const
{
    if (sourceModel() == 0 || !index.isValid()) {
        qDebug()<<"No source model || invalid index";
        return QVariant();
    }
    QModelIndex source_index;
    int col = index.column() - sourceModel()->columnCount();
    if (col < 0) {
        source_index = mapToSource(index);
        //qDebug()<<"source column"<<col<<sourceModel()->columnCount();
    } else {
        source_index = mapToSource(this->index(index.row(), 0));
        //qDebug()<<"proxy column"<<col<<sourceModel()->columnCount();
    }
    if (!source_index.isValid()) {
        qDebug()<<"index valid but source index not valid:"<<index;
        return QVariant();
    }
    QVariant r;
    if (col < 0) {
        r = sourceModel()->data(source_index, role);
    } else if (col == 0) {
        source_index = source_index.parent();
        if (source_index.isValid()) {
            r = sourceModel()->data(source_index, role);
        }
    }
    //qDebug()<<index<<r;
    return r;
}

bool FlatProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (sourceModel() == 0) {
        return false;
    }
    QModelIndex source_index = mapToSource(index);
    if (index.isValid() && !source_index.isValid()) {
        return false;
    }
    return sourceModel()->setData(source_index, value, role);
}

QVariant FlatProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (sourceModel() == 0) {
        return QVariant();
    }
    int sec = section - sourceModel()->columnCount();
    if (sec < 0) {
        return sourceModel()->headerData(section, orientation, role);
    }
    if (sec == 0) {
        return tr("Parent");
    }
    return QVariant();
}

bool FlatProxyModel::setHeaderData(int section, Qt::Orientation orientation,
                                          const QVariant &value, int role)
{
    if (sourceModel() == 0) {
        return false;
    }
    //TODO
    return sourceModel()->setHeaderData(section, orientation, value, role);
}

QMimeData *FlatProxyModel::mimeData(const QModelIndexList &indexes) const
{
    if (sourceModel() == 0) {
        return 0;
    }
    QModelIndexList source_indexes;
    for (int i = 0; i < indexes.count(); ++i) {
        source_indexes << mapToSource(indexes.at(i));
    }
    return sourceModel()->mimeData(source_indexes);
}

QStringList FlatProxyModel::mimeTypes() const
{
    if (sourceModel() == 0) {
        return QStringList();
    }
    return sourceModel()->mimeTypes();
}

Qt::DropActions FlatProxyModel::supportedDropActions() const
{
    if (sourceModel() == 0) {
        return 0;
    }
    return sourceModel()->supportedDropActions();
}

bool FlatProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                         int row, int column, const QModelIndex &parent)
{
    if (sourceModel() == 0) {
        return false;
    }
    if ((row == -1) && (column == -1))
        return sourceModel()->dropMimeData(data, action, -1, -1, mapToSource(parent));
    int source_destination_row = -1;
    int source_destination_column = -1;
    QModelIndex source_parent;
    if (row == rowCount(parent)) {
        source_parent = mapToSource(parent);
        source_destination_row = sourceModel()->rowCount(source_parent);
    } else {
        QModelIndex proxy_index = index(row, column, parent);
        QModelIndex source_index = mapToSource(proxy_index);
        source_destination_row = source_index.row();
        source_destination_column = source_index.column();
        source_parent = source_index.parent();
    }
    return sourceModel()->dropMimeData(data, action, source_destination_row,
                                  source_destination_column, source_parent);
}

bool FlatProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

bool FlatProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    //TODO
    return false;
}


/*!
   Returns the source model index corresponding to the given \a
   proxyIndex from the sorting filter model.

   \sa mapFromSource()
*/
QModelIndex FlatProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (! proxyIndex.isValid()) {
        return QModelIndex();
    }
    QModelIndex source_index = m_sourceIndexList.value(proxyIndex.row());
    if (proxyIndex.column() != 0) {
        source_index = sourceModel()->index(source_index.row(), proxyIndex.column(), source_index.parent());
    }
    //qDebug()<<proxyIndex<<"->"<<source_index;
    return source_index;
}

/*!
    Returns the model index in the FlatProxyModel given the \a
    sourceIndex from the source model.

    \sa mapToSource()
*/
QModelIndex FlatProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (! sourceIndex.isValid()) {
        return QModelIndex();
    }
    QPersistentModelIndex idx = sourceIndex;
    if (idx.column() != 0) {
        // we only map indices with column 0
        idx = sourceModel()->index(idx.row(), 0, idx.parent());
    }
    QModelIndex proxy_index = index(m_sourceIndexList.indexOf(idx), sourceIndex.column());
    //qDebug()<<sourceIndex<<"->"<<proxy_index;
    return proxy_index;
}

QItemSelection FlatProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    return QAbstractProxyModel::mapSelectionToSource(proxySelection);
}

QItemSelection FlatProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    return QAbstractProxyModel::mapSelectionFromSource(sourceSelection);
}

void FlatProxyModel::initiateMaps(const QModelIndex &sourceParent)
{
    if (! sourceParent.isValid()) {
        m_sourceIndexList.clear();
        m_sourceIndexMap.clear();
    }
    QAbstractItemModel *m = sourceModel();
    if (m == 0) {
        qDebug() << "No source model";
        return;
    }
    int count = m->rowCount(sourceParent);
    for (int row = 0; row < count; ++row) {
        QPersistentModelIndex idx = m->index(row, 0, sourceParent);
        //qDebug()<<"map:"<<sourceParent<<row<<idx;
        if (idx.isValid()) { // fail safe
            m_sourceIndexList.append(idx);
            m_sourceIndexMap.insert(idx.parent(), idx);

            initiateMaps(idx);
        }
    }
    //qDebug()<<"source index list="<<m_sourceIndexList;
}


} // namespace QuickChat
