#include "plugpackagemodel.h"
#include <QDebug>

plugPackageModel::plugPackageModel(QObject* parent)
: QAbstractItemModel(parent), m_root_node(new plugPackageItem)
{
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
						nodeFromIndex(parent)->getChildData(row));
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
	return parentNode->childrenCount();
}

int plugPackageModel::columnCount(const QModelIndex& parent) const
{
	return 1; //from example (=
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
		switch(role) {
			case Qt::DisplayRole:
				return node->getItemData()->packageItem.properties.value("name");
			case Qt::DecorationRole:
				return node->getItemData()->icon;
			case InstalledRole:
				return node->getItemData()->attribute;
			case CategoryRole:
				return m_category_nodes.contains(node->item_name);
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
	
	if (orientation == Qt::Horizontal)
		return tr("Packages");
	else
		return QString("Row %1").arg(section);
}

void plugPackageModel::addItem(const ItemData& item, const QString& name) {
	plugPackageItem *category_node = m_category_nodes.value(item.packageItem.properties.value("type"));
	if (!category_node) {
		ItemData category_item = ItemData (group,QIcon(":/icons/hi64-action-package.png"));
		category_item.packageItem.properties.insert("name", item.packageItem.properties.value("type"));
		category_node = new plugPackageItem (category_item,
										item.packageItem.properties.value("type"));
 		m_category_nodes.insert(item.packageItem.properties.value("type"),category_node);
		beginInsertRows(QModelIndex(),m_root_node->childrenCount(),m_root_node->childrenCount());
		m_root_node->addChild(category_node, m_root_node->childrenCount());
		endInsertRows();
	}
	if (m_packages.contains(name)) {
		ItemData update_item = item;
		if (m_packages.value(name)->getItemData()->attribute == installed) {
			update_item.attribute = isUpgradable; //FIXME for testing!
		}
		m_packages.value(name)->setItem(update_item,name);
	}
	else {
		plugPackageItem *node = new plugPackageItem (item, name);
		m_packages.insert(name,node);
		qDebug () << "insert item:" << name;
		beginInsertRows(createIndex(category_node->childrenCount(), 0, category_node),category_node->childrenCount(),category_node->childrenCount());
		category_node->addChild(node,category_node->childrenCount());
		endInsertRows();
	}
	return;
}

void plugPackageModel::clear() {
	reset();
	delete(m_root_node);
	m_category_nodes.clear();
	qDebug() << m_packages;
	m_packages.clear();
 	m_root_node = new plugPackageItem;
	qDebug () << "clear";	
}

