#include "plugpackageitem.h"

plugPackageItem::plugPackageItem(const ItemData& data, const QString& name)
: parent(0)
{
	item_data = data;
	item_name = name;
}

plugPackageItem::~plugPackageItem() {
	qDeleteAll(children);
}

void plugPackageItem::addChild(plugPackageItem *data, const int& id) {
	data->parent = this;
	children.insert(id,data);
}

void plugPackageItem::removeChild(const QString& id, int number) {

}
