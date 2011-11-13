/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
	#include "plugpackagemodel.h"
	#include "utils/plugversion.h"
	#include <QDebug>
	#include <qutim/iconmanagerinterface.h>

	plugPackageModel::plugPackageModel(bool isGrouped, QObject* parent)
			: QAbstractItemModel(parent), m_root_node(new plugPackageItem)
	{
		this->isGrouped = isGrouped;
	}

	plugPackageModel::~plugPackageModel() {
		delete(m_root_node);
	}

	// void plugPackageModel::setRootNode(plugPackageItem* plug_package_item) {
	// 	reset();
	// 	delete(m_root_node);
	// 	m_root_node = plug_package_item;
	// }

	QModelIndex plugPackageModel::index(int row, int column, const QModelIndex& parent) const {
		if (!m_root_node)
			return QModelIndex();
		return createIndex(	row,
							column,
							nodeFromIndex(parent)->Child(row));
	}

	plugPackageItem *plugPackageModel::nodeFromIndex(const QModelIndex& index) const {
		if (index.isValid())
			return static_cast<plugPackageItem *>(index.internalPointer());
		else
			return m_root_node;
	}

	int plugPackageModel::rowCount(const QModelIndex& parent) const {
		plugPackageItem *parentNode = nodeFromIndex(parent);
		if (!parentNode)
			return 0;
		return parentNode->childCount();
	}

	int plugPackageModel::columnCount(const QModelIndex& parent) const
	{
		return 1;
	}

	QModelIndex plugPackageModel::parent(const QModelIndex& child) const {
		plugPackageItem *node = nodeFromIndex(child);
		if (!node)
			return QModelIndex();
		plugPackageItem *parentNode = node->getParent();
		if (!parentNode)
			return QModelIndex();
		plugPackageItem *grandParentNode = parentNode->getParent();
		if (!grandParentNode)
			return QModelIndex();
		int row = grandParentNode->indexOf(parentNode);
		return createIndex(row, child.column(), parentNode);
	}

	QVariant plugPackageModel::data(const QModelIndex& index, int role) const {
		plugPackageItem *node = nodeFromIndex(index);
		if (index.column()==0)	{
			switch (role) {
				case Qt::DisplayRole:
					return node->getItemData()->packageItem.properties.value("name");
				case Qt::DecorationRole:
					return node->getItemData()->icon;
				case InstalledRole:
					return node->getItemData()->attribute;
				case CheckedRole:
					return node->getItemData()->checked;
				case CategoryRole:
					return node->getItemData()->type;
				case SummaryRole:
					return node->getItemData()->packageItem.properties.value("shortdesc");
				default:
				return QVariant();
			}
		}
		return QVariant();
	}

	bool plugPackageModel::hasChildren(const QModelIndex& parent) const {
		return QAbstractItemModel::hasChildren(parent);
	}


	QVariant plugPackageModel::headerData(int section, Qt::Orientation orientation, int role) const {
		if (role != Qt::DisplayRole)
			return QVariant();

		if (orientation != Qt::Horizontal)
			return QString("Row %1").arg(section);
		else
			return tr("Packages");
	}

	void plugPackageModel::addItem(ItemData *item) {
		if (!isGrouped) {
			if (m_packages.contains(item->name)) {
				plugVersion currentVersion (m_packages.value(item->name)->getItemData()->packageItem.properties.value("version"));
				plugVersion replaceVersion (item->packageItem.properties.value("version"));
				if (replaceVersion>currentVersion) {
					if ((m_packages.value(item->name)->getItemData()->attribute == installed))
						item->attribute = isUpgradable;
					m_packages.value(item->name)->setItem(item);
				}
			}
			else {
				plugPackageItem *node = new plugPackageItem (item);
				m_packages.insert(item->name,node);
				beginInsertRows(QModelIndex(),m_root_node->childCount(),m_root_node->childCount());
				m_root_node->appendChild(node);
				endInsertRows();
			}
		}
		else {
			plugPackageItem *category_node = m_category_nodes.value(item->packageItem.properties.value("type"));
			if (!category_node) {
				ItemData *category_item = new ItemData (group,qutim_sdk_0_2::SystemsCity::IconManager()->getIcon("package"));
				category_item->packageItem.properties.insert("name", item->packageItem.properties.value("type"));
				category_node = new plugPackageItem (category_item);
				m_category_nodes.insert(item->packageItem.properties.value("type"),category_node);
				beginInsertRows(QModelIndex(),m_root_node->childCount(),m_root_node->childCount());
				m_root_node->appendChild(category_node);
				endInsertRows();
			}
			if (m_packages.contains(item->name)) {
				plugVersion currentVersion (m_packages.value(item->name)->getItemData()->packageItem.properties.value("version"));
				plugVersion replaceVersion (item->packageItem.properties.value("version"));
				if (replaceVersion>currentVersion) {
					if ((m_packages.value(item->name)->getItemData()->attribute == installed))
						item->attribute = isUpgradable;
					m_packages.value(item->name)->setItem(item);
				}
			}
			else {
				plugPackageItem *node = new plugPackageItem (item);
				m_packages.insert(item->name,node);
				QModelIndex index = createIndex(m_root_node->indexOf(category_node),0,category_node);
				beginInsertRows(index,category_node->childCount(),category_node->childCount());
				category_node->appendChild(node);
				endInsertRows();
			}
		}
		return;
	}

	void plugPackageModel::clear() {
		reset();
		delete(m_root_node);
		m_category_nodes.clear();
		m_checked_packages.clear();
		m_packages.clear();
		m_root_node = new plugPackageItem;
	}

	QHash<QString, plugPackageItem *> &plugPackageModel::getCheckedPackages() {
		return m_checked_packages;
	}

	bool plugPackageModel::setData(const QModelIndex &index, const QVariant &value, int role) {
		if (role != CheckedRole)
			return false;
		plugPackageItem *node = nodeFromIndex(index);
		if (node->getItemData()->type==group)
			return false;
		int installedRole = node->getItemData()->attribute;
		int checkedRole = node->getItemData()->checked;
		QString name = node->getItemData()->packageItem.properties.value("type") + "/" + node->getItemData()->packageItem.properties.value("name");
		qDebug() << name;
		switch (checkedRole) {
		case unchecked:
			switch (installedRole) {
					case isInstallable:
				node->getItemData()->checked = markedForInstall;
				break;
					case isUpgradable:
				node->getItemData()->checked = markedForUpgrade;
				break;
					case isDowngradable:
				node->getItemData()->checked = markedForDowngrade;
				break;
					case installed:
				node->getItemData()->checked = markedForRemove;
				break;
			}
			m_checked_packages.insert(name,node);
			break;
		case markedForInstall:
			node->getItemData()->checked = unchecked;
			m_checked_packages.remove(name);
			break;
		case markedForUpgrade:
			node->getItemData()->checked = markedForRemove;
			break;
		case markedForRemove:
			node->getItemData()->checked = unchecked;
			m_checked_packages.remove(name);
			break;
		}
		emit dataChanged(createIndex(0, 1), index);
		return false;
	}
	void plugPackageModel::uncheckAll() {
		QHash<QString,plugPackageItem *>::const_iterator it = m_checked_packages.begin();
		for (it = m_checked_packages.begin(); it!=m_checked_packages.end();it++) {
			it.value()->getItemData()->checked = unchecked;
		}
		m_checked_packages.clear();
	}

	void plugPackageModel::upgradeAll() {
		QHash<QString,plugPackageItem *>::const_iterator it = m_packages.begin();
		for (it = m_packages.begin(); it!=m_packages.end();it++) {
			if (it.value()->getItemData()->attribute == isUpgradable && !m_checked_packages.contains(it.key())) {
				m_checked_packages.insert(it.key(),it.value());
				it.value()->getItemData()->checked == markedForInstall;
				qDebug() << it.value()->getItemData()->name << "Marked for upgrade";
			}
		}
	}

