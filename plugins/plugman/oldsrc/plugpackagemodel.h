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

#ifndef PLUGPACKAGEMODEL_H
#define PLUGPACKAGEMODEL_H

#include <QAbstractItemModel>
#include "plugpackage.h"
#include "plugpackageitem.h"

class plugPackageModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    plugPackageModel(bool isGrouped = false, QObject *parent = 0);
    ~plugPackageModel();
// 	void setRootNode (plugPackageItem *plug_package_item);
    enum {
        NameRole = Qt::UserRole,
        InstalledRole, //installed, isUpgradable, etc
        IconRole,
        IdRole,
        GroupRole,
        CheckedRole,
        SummaryRole,
        CategoryRole //Категория или пакет?
    };
    QModelIndex index (	int row, int column,
                        const QModelIndex &parent) const;
    QModelIndex parent (const QModelIndex &child) const;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    void addItem(ItemData* item);
    void clear();
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QHash<QString, plugPackageItem *> &getCheckedPackages ();
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
private:
    plugPackageItem *m_root_node;
    plugPackageItem *nodeFromIndex(const QModelIndex &index) const;
    QHash<QString, plugPackageItem *> m_category_nodes; //! категории пакетов
    QHash<QString, plugPackageItem *> m_packages; //!список всех пакетов
    QHash<QString, plugPackageItem *> m_checked_packages;
    bool isGrouped;
public slots:
    void uncheckAll ();
    void upgradeAll ();
};

#endif // PLUGPACKAGEMODEL_H

