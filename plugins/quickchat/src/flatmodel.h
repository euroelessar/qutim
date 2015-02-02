/* This file is part of the KDE project
  Copyright(C)2010 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or(at your option)any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef KPTFLATPROXYMODEL_H
#define KPTFLATPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QStandardItemModel>
#include <QQmlPropertyMap>

class PersistentModelIndex;
class QModelIndex;
class QItemSelection;

/// The main namespace
namespace QuickChat
{

class FlatProxyModelData;

/**
    FlatProxyModel is a proxy model that makes a tree source model flat.

    This might be useful to present data from a tree model in e.g. a table view or a report.

    Note that the source model should have the same number of columns for all parent indices,
    since a flat model obviously have the same number of columns for all indices.
    If this is not the case, the behavior is undefined.

    The row sequence of the flat model is the same as if the source model was fully expanded.

    The flat model adds a Parent column at the end of the source model columns,
    to make it possible to access the parent index's data at column 0.
*/
class FlatProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit FlatProxyModel(QObject *parent = 0);

    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex)const;
    virtual QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection)const;
    virtual QItemSelection mapSelectionToSource(const QItemSelection &proxySelection)const;
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex)const;
    virtual void setSourceModel(QAbstractItemModel * sourceModel);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex())const;
    QModelIndex parent(const QModelIndex &child)const;
    int rowCount(const QModelIndex &parent = QModelIndex())const;
    int columnCount(const QModelIndex &parent  = QModelIndex())const;
    bool hasChildren(const QModelIndex &parent = QModelIndex())const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    QMimeData *mimeData(const QModelIndexList &indexes)const;
    QStringList mimeTypes()const;
    Qt::DropActions supportedDropActions()const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent = QModelIndex());

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Q_INVOKABLE QObject *rowData(int row);

public slots:
    void onSourceDataChanged(const QModelIndex &source_top_left,
                           const QModelIndex &source_bottom_right);
    void onSourceHeaderDataChanged(Qt::Orientation orientation, int start, int end);

    void onSourceReset();

    void onSourceLayoutAboutToBeChanged();
    void onSourceLayoutChanged();

    void onSourceRowsAboutToBeInserted(const QModelIndex &source_parent,
                                        int start, int end);
    void onSourceRowsInserted(const QModelIndex &source_parent,
                               int start, int end);
    void onSourceRowsAboutToBeRemoved(const QModelIndex &source_parent,
                                       int start, int end);
    void onSourceRowsRemoved(const QModelIndex &source_parent,
                              int start, int end);

    void onSourceRowsAboutToBeMoved(const QModelIndex &source_parent,
                                  int start, int end, const QModelIndex &destParent, int destStart);
    void onSourceRowsMoved(const QModelIndex &source_parent,
                                  int start, int end, const QModelIndex &destParent, int destStart);

protected:
    int mapFromSourceRow(const QModelIndex &sourceIndex)const;
    int mapToSourceRow(const  QModelIndex &sourceIndex)const;

private slots:
    void initiateMaps(const QModelIndex &sourceParent = QModelIndex());
    void onSourceModelDestroyed();

private:
    friend class FlatProxyModelData;
    /// List of sourceIndexes
    QList<QPersistentModelIndex> m_sourceIndexList;
    /// Map of sourceIndexes(parent, index)
    QMultiMap<QPersistentModelIndex, QPersistentModelIndex> m_sourceIndexMap;
    QHash<int, QByteArray> m_roleNames;
};

class FlatProxyModelData : public QQmlPropertyMap
{
    Q_OBJECT
public:
    FlatProxyModelData(int row, FlatProxyModel *model);
    ~FlatProxyModelData();

private:
    const QModelIndex m_index;
};

} //namespace QuickChat


#endif
