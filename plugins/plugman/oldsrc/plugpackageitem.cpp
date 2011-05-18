#include "plugpackageitem.h"
#include <QDebug>
#include "plugdownloader.h"
#include "hacks/plughacks.h"

plugPackageItem::plugPackageItem(ItemData *data)
        : parent(0)
{
    item_data = data;
    if (item_data->type==buddy&&!item_data->packageItem.properties.value("icon").isEmpty()) {
        plugDownloader *loader = new plugDownloader(plugPathes::getCachePath() + "icons/");
        downloaderItem item =
        {
            item_data->packageItem.properties.value("icon"),
            item_data->packageItem.properties.value("name")
            + "-"
            + item_data->packageItem.properties.value("version")
            + ".png" //FIXME для тестирования
        };
        loader->addItem(item);
		qDebug() << item.url << item.filename;
		connect(loader,SIGNAL(downloadFinished(QList<downloaderItem>)),SLOT(iconDownloaded(QList<downloaderItem>)));
		loader->startDownload();
    }
}

plugPackageItem::plugPackageItem()
        : parent(0), item_data(new ItemData)
{
}


plugPackageItem::~plugPackageItem() {
    qDeleteAll(childItems);
    delete (item_data);
}

void plugPackageItem::appendChild(plugPackageItem* data) {
    data->parent = this;
    childItems.append(data);
}

void plugPackageItem::removeChild(const int& row) {
    childItems.removeAt(row);
}

void plugPackageItem::setItem(ItemData *data)
{
    delete(item_data);
    item_data = data;
}

plugPackageItem* plugPackageItem::Child(const int& row)
{
    return childItems.value(row);
}

int plugPackageItem::childCount()
{
    return childItems.count();
}

int plugPackageItem::indexOf(plugPackageItem* item)
{
    return childItems.indexOf(item);
}

ItemData* plugPackageItem::getItemData()
{
    return item_data;
}

plugPackageItem* plugPackageItem::getParent()
{
    return parent;
}

int plugPackageItem::row() const
{
    if (parent)
        return parent->childItems.indexOf(const_cast<plugPackageItem*>(this));
    return 0;
}


void plugPackageItem::iconDownloaded(const QList< downloaderItem >& items)
{
    item_data->icon = QIcon(items.first().filename);
}

