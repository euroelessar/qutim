#include "plugpackageitem.h"
#include <QDebug>

plugPackageItem::plugPackageItem(const ItemData& data)
: parent(0)
{
	item_data = data;
}

plugPackageItem::plugPackageItem()
: parent(0)
{

}


plugPackageItem::~plugPackageItem() {
// 	qDebug() << "delete :" << this->children;
	qDeleteAll(children);
}

void plugPackageItem::addChild(plugPackageItem *data, const int& id) {
	data->parent = this;
	children.insert(id,data);
// 	qDebug () << "add child:" << children;
}

void plugPackageItem::removeChild(const int& id) {
	//FIXME так работать не буит ^_^
	children.remove(id);
}

void plugPackageItem::setItem(const ItemData& data)
{
	item_data = data;
}
