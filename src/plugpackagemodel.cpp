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

int plugPackageModel::columnCount(const QModelIndex& parent) const
{
	return 2; //from example (=
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
}

QVariant plugPackageModel::headerData(int section, Qt::Orientation orientation, int role) const {
	return QAbstractItemModel::headerData(section, orientation, role);
}

void plugPackageModel::addItem(const ItemData& item, const QString& name) {
	plugPackageItem *node = new plugPackageItem (item, name);
}

