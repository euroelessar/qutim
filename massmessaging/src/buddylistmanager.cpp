#include "buddylistmanager.h"
#include "manager.h"
#include "jsonfile.h"

BuddyListManager::BuddyListManager ( const QString& path )
        : path(path)
{

}


QList< SendItem > BuddyListManager::Load() const
{
    QList< SendItem > items;
    QVariant variant;
    JsonFile jsonFile (path);
    jsonFile.load(variant);
    if (variant.type()!=QVariant::List) {
        qCritical() << "Unable to load list"; //TODO add native qutIM notification
        return items;
    }
    QVariantList list (variant.toList());
    foreach (const QVariant &item,list) {
        items << toSendItem(item);
    }
    return items;
}


void BuddyListManager::Save ( const QList< SendItem >& items )
{
    QVariantList list;
    foreach (const SendItem &item, items) {
        list.append(toQVariant(item));
    }
    JsonFile jsonFile (path);
    jsonFile.save(QVariant(list));
}


QVariant BuddyListManager::toQVariant ( const SendItem& item ) const
{
    QVariantMap var;
    var.insert("account",item.treeModelItem.m_account_name);
    var.insert("protocol",item.treeModelItem.m_protocol_name);
    var.insert("group",item.treeModelItem.m_parent_name);
    var.insert("reciever",item.treeModelItem.m_item_name); //FIXME
    var.insert("name",item.name);
    var.insert("checked",item.isChecked);
    qDebug() << item.name << item.treeModelItem.m_item_name << item.treeModelItem.m_parent_name;
    return QVariant(var);
}


SendItem BuddyListManager::toSendItem ( const QVariant& variant ) const
{
    if (variant.type()!=QVariant::Map) {
        qWarning() << "Invalid item";
        return SendItem();
    }
    QVariantMap m(variant.toMap());
    TreeModelItem treeItem;
    treeItem.m_account_name = m.value("account").toString();
    treeItem.m_parent_name = m.value("group","unknown").toString();
    treeItem.m_item_name = m.value("reciever").toString();
    treeItem.m_protocol_name = m.value("protocol").toString();
    treeItem.m_item_type = TreeModelItem::Buddy;
    if (treeItem.m_account_name.isEmpty()||treeItem.m_item_name.isEmpty()) {
         qWarning() << "Invalid item";
        return SendItem();
    }
    SendItem item;
    item.treeModelItem = treeItem;
    item.name = m.value("name","unknown").toString();
    item.isChecked = m.value("checked",false).toBool();
    qDebug () << item.name << item.treeModelItem.m_parent_name;
    return item;
}

