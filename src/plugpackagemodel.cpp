#include "plugpackagemodel.h"

#define Q_VALID_TYPE(item) (item.m_item_type <= 2)
#define Q_CHECK_TYPE(item) \
	if( !Q_VALID_TYPE(item) ) \
		return
#define Q_INDEX(item) createIndex(item->childNumber(), 0, item)

PlugPackageModel::PlugPackageModel(QObject* parent) : QAbstractItemModel(parent), m_root_item(new plugPackageItem)
{

}

QModelIndex PlugPackageModel::index(int row, int column, const QModelIndex& parent) const {
	Q_UNUSED(column);
	plugPackageItem *item = getItem(parent);
	if(!item)
		return QModelIndex();
	item = item->getChild(row);
	return item ? Q_INDEX(item) : QModelIndex();
}

QModelIndex PlugPackageModel::parent(const QModelIndex& child) const {
	plugPackageItem *item = getItem(child);
	if(!item)
		return QModelIndex();
	item = item->parent();
	if(!item || !item->getItem())
		return QModelIndex();
	return Q_INDEX(item);
}

int PlugPackageModel::rowCount(const QModelIndex& parent) const {
	plugPackageItem *item = getItem(parent);
	return item ? item->childCount() : 0;	
}

bool PlugPackageModel::hasChildren(const QModelIndex& parent) const {
return QAbstractItemModel::hasChildren(parent);
}

QVariant PlugPackageModel::data(const QModelIndex& index, int role) const {
	plugPackageItem *item = getItem(index);
 	if(!item)
 		return QVariant();
 	packageInfo *item = item->getItem();
 	if(!item)
 		return QVariant();
 	switch(role)
 	{
 	case Qt::DisplayRole:
 		return data->name.isEmpty() ? data->item.m_item_name : data->name;
 	case Qt::DecorationRole:
 		return QVariant();//FIXME
 	case Qt::UserRole:
 		return reinterpret_cast<qptrdiff>(item);
 	default:
 		return QVariant();
 	}
	return QVariant();
}

void PlugPackageModel::addItem(const packageInfo& item) {
	Q_CHECK_TYPE(item);
	if(getItem(item)) return;
	plugPackageItem *data_item = new plugPackageItem(item);
	addItem(item, data_item);
}

void PlugPackageModel::addItem(const packageInfo& item, plugPackageItem* data_item) {

}

plugPackageItem* PlugPackageModel::getItem(const QModelIndex& index) const {

}

plugPackageItem* PlugPackageModel::getItem(const packageInfo& item) const {

}

QStringList PlugPackageModel::getItemChildren(const packageInfo& item) {

}

void PlugPackageModel::moveItem(const packageInfo& old_item, const packageInfo& new_item) {

}

void PlugPackageModel::removeItem(const packageInfo& item) {

}

void PlugPackageModel::setItemAttribute(const packageInfo& item, packageAttribute type, bool on) {

}

void PlugPackageModel::setItemDescription(const packageInfo& item, const QVector< QVariant >& text) {

}

void PlugPackageModel::setItemIcon(const packageInfo& item, const QIcon& icon, int position) {

}

void PlugPackageModel::setItemName(const packageInfo& item, const QString& name) {

}

void PlugPackageModel::setItemShortDesc(const packageInfo& item, const QString& shortdesc) {

}

void PlugPackageModel::setItemVisibility(const packageInfo& item, int flags) {

}













