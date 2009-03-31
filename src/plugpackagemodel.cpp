#include "plugpackagemodel.h"

plugPackageModel::plugPackageModel(QObject* parent)
: QAbstractItemModel(parent)
{
	m_root_node = 0;
}

plugPackageModel::~plugPackageModel() {
	delete(m_root_node);
}

void plugPackageModel::setRootNode(plugPackageItem* plug_package_item) {
	delete(m_root_node);
	m_root_node = plug_package_item;
	reset();
}

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

<<<<<<< .mine
int plugPackageModel::columnCount(const QModelIndex& parent) const {
	return 2; //from example (=
=======
QVariant PlugPackageModel::data(const QModelIndex& index, int role) const {
	plugPackageItem *item = getItem(index);
	ItemData *data = item->getData();
	if(!item)
		return QVariant();
	if(!data)
		return QVariant();
// 	switch(role)
// 	{
// 		case Qt::DisplayRole:
// 			return data->name.isEmpty() ? data->item.m_item_type == 1 ? "Not in list" : data->item.m_item_name : data->name;
// 		case Qt::DecorationRole:
// 			return data->icons.count()==0 ? QVariant() : data->icons[0];
// 		case Qt::UserRole:
// 			return reinterpret_cast<qptrdiff>(data);
// 		case Qt::ToolTipRole:
// 			return m_plugin_system->getItemToolTip(data->item);
// 		default:
// 			return QVariant();
// 	}
	return QVariant();
>>>>>>> .r43
}

QModelIndex plugPackageModel::parent(const QModelIndex& child) const {
	plugPackageItem *node = nodeFromIndex(child);
	if (!node)
		return QModelIndex();
	plugPackageItem *parentNode = node->parent;
	if (!parentNode)
		return QModelIndex();
	plugPackageItem *grandParentNode = parentNode->parent;
	if (!grandParentNode)
		return QModelIndex();
	int row = grandParentNode->indexOf(parentNode);
	return createIndex(row, child.column(), parentNode);
}

<<<<<<< .mine
QVariant plugPackageModel::data(const QModelIndex& index, int role) const {
	if (role != Qt::DisplayRole)
		return QVariant();
	plugPackageItem *node = nodeFromIndex(index);
	if (index.column()==0)	{
		return tr("package");
		}
	else if (index.column()==1) {
		return node->item_name;
	}
	return QVariant();
=======
void PlugPackageModel::addItem(const packageInfo& item, plugPackageItem* data_item) {
 	QHash<QString, plugPackageItem *> &category = m_items[item.properties["type"]];
 	if (category.isEmpty()) {
		category.insert(item.properties["type"],data_item);
		beginInsertRows(QModelIndex(), m_root_item->childCount(),m_root_item->childCount());
		m_root_item->addChild(item.properties["type"],data_item);
		endInsertRows();
		return;
 	}
	plugPackageItem *package = category.value(item.id, 0); //блин надо у элессара уточнить как оно работает
	if (!package)	{
		delete data_item;
		return;
		}
	beginInsertRows(Q_INDEX(package),package->childCount(),package.childCount());
	package->addChild(item.id, data_item);
	endInsertRows();
// 	return;
// 	category.insert()
// 	if(item.m_item_type == 2)
// 	{
// 		protocol.insert(item.m_account_name, data_item);
// 		beginInsertRows(QModelIndex(), m_root_item->childCount(), m_root_item->childCount());
// 		m_root_item->addChild(item.m_protocol_name+"."+item.m_account_name, data_item);
// 		endInsertRows();
// 		return;
// 	}
// 	DataItem *account = protocol.value(item.m_account_name, 0);
// 	if(!account)
// 	{
// 		delete data_item;
// 		return;
// 	}
// 	if(item.m_item_type == 1)
// 	{
// 		beginInsertRows(Q_INDEX(account), account->childCount(), account->childCount());
// 		account->addChild(item.m_item_name, data_item);
// 		endInsertRows();
// 		return;
// 	}
// 	DataItem *group = account->getChild(item.m_parent_name);
// 	if(!group)
// 	{
// 		delete data_item;
// 		return;
// 	}
// 	beginInsertRows(Q_INDEX(group), group->childCount(), group->childCount());
// 	group->addChild(item.m_item_name, data_item);
// 	endInsertRows();
>>>>>>> .r43
}

QVariant plugPackageModel::headerData(int section, Qt::Orientation orientation, int role) const {
	return QAbstractItemModel::headerData(section, orientation, role);
}

void plugPackageModel::addItem(const ItemData& item, const QString& name) {
	plugPackageItem *node = new plugPackageItem (item, name);
}

