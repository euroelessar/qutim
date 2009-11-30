#include "manager.h"
#include <QTimer>
#include <qutim/iconmanagerinterface.h>

SendItem::SendItem(const TreeModelItem& item, const QString& name)
{
    this->treeModelItem = item;
    this->name = name;
}

SendItem::SendItem()
{
}


Manager::Manager(QObject* parent)
        : QObject(parent),m_root_item(new QTreeWidgetItem)
{
    m_root_item->setText(0,tr("Accounts"));
    m_root_item->setFlags(m_root_item->flags() | Qt::ItemIsUserCheckable);
    m_root_item->setCheckState(0, Qt::Unchecked);
    QFont font;
    font.setBold(true);
    m_root_item->setFont(0,font);
    m_root_item->setIcon(0,SystemsCity::PluginSystem()->getIcon("qutim"));
    QTreeWidgetItem *unknown = new QTreeWidgetItem();
    unknown->setFlags(unknown->flags() | Qt::ItemIsUserCheckable);
    unknown->setCheckState(0,Qt::Unchecked);
    unknown->setText(0,tr("Unknown"));
    m_root_item->addChild(unknown);
    m_groups.insert("unknown",unknown);
    connect(this,SIGNAL(finished()),SLOT(endSending()));
}

Manager::~Manager()
{
    delete m_root_item;
    qDeleteAll(m_buddy_items);
}


QTreeWidgetItem *Manager::getRootItem() {
    return m_root_item;
}

void Manager::addItem(const TreeModelItem& item, const QString& name,const bool &isChecked)
{
    QTreeWidgetItem *tree_item = new QTreeWidgetItem();
    tree_item->setFlags(tree_item->flags() | Qt::ItemIsUserCheckable);
    tree_item->setCheckState(0,Qt::Unchecked);
    switch (item.m_item_type) {
    case 2: //Accounts
        tree_item->setText(0,name);        
        tree_item->setIcon(0,
                           SystemsCity::IconManager()->getIcon(item.m_protocol_name.toLower(),qutim_sdk_0_2::IconInfo::Protocol)
                           );
        m_root_item->addChild(tree_item);
        m_accounts.insert(item.m_account_name,tree_item);
        break;
    case 1: //Groups
        tree_item->setText(0,name);
        m_accounts.value(item.m_account_name,m_groups.value("unknown"))->addChild(tree_item);
        m_groups.insert(item.m_protocol_name+"/"+item.m_item_name,tree_item);
        break;
    case 0: //Buddy items
        tree_item->setText(0,name.isEmpty() ? item.m_item_name : name);
        tree_item->setToolTip(0,item.m_item_name); //FIXME hack а по русски костыль
        tree_item->setCheckState(0,isChecked ? Qt::Checked : Qt::Unchecked);
        m_groups.value(item.m_protocol_name+"/"+item.m_parent_name,m_groups.value("unknown"))->addChild(tree_item);
        SendItem *m_send_item = new SendItem(item,name);
        m_buddy_items.insert(item.m_item_name,m_send_item);
        break;
    }
}

void Manager::removeItem(const TreeModelItem& item)
{
    //TODO
    Q_UNUSED(item);
}

void Manager::beginSending(const QString& message, const uint &delay)
{
    if (message.isEmpty()) {
        emit finished(false);
        TreeModelItem item;
        SystemsCity::PluginSystem()->customNotification(item,tr("Error: message is empty"));
        return;
    }

    this->message = message;
    this->delay = delay;
    createSendingQueue();
    sendMessage();
}

void Manager::endSending()
{
    m_send_items.clear();
}

void Manager::sendMessage()
{
    if (m_send_items.isEmpty()) {
        emit finished(true);
        return;
    }
    SendItem *send_item = m_send_items.dequeue();
    SystemsCity::PluginSystem()->sendCustomMessage(send_item->treeModelItem,parseMessage(send_item),true);
    emit updateProgressBar(count - m_send_items.count(),count,send_item->name);
    QTimer::singleShot(delay*1000, this, SLOT(sendMessage()));
}

QString Manager::parseMessage(SendItem* item)
{
    QString parsed_message = message;
    parsed_message.replace("{reciever}",item->name);
    parsed_message.replace("{sender}",SystemsCity::ProfileName());
    parsed_message.replace("{time}",QTime::currentTime().toString());
    return parsed_message;
}


void Manager::createSendingQueue()
{
    m_send_items.clear();
    qDebug() << "Begin create queue";
    foreach (QTreeWidgetItem *group,m_groups) {
        qDebug () << "group:" << group->text(0);
        for ( int i = 0; i != group->childCount(); i++)
        {
            qDebug() << "View item:" << i  << group->child(i)->toolTip(0) << group->child(i)->checkState(0);
            if (group->child(i)->checkState(0)==Qt::Checked) {
                m_send_items.append(m_buddy_items.value(group->child(i)->toolTip(0)));
                qDebug() << "Item added" << group->child(i)->toolTip(0);
            }
        }
    }
    count = m_send_items.count();
}


void Manager::addCustomBuddyList ( const QList< SendItem >& items)
{
    foreach (SendItem sendItem, items) {
        if (!m_accounts.contains(sendItem.treeModelItem.m_account_name)) {
            SystemsCity::PluginSystem()->customNotification(sendItem.treeModelItem,
                                                            tr("Error: unknown account : %1").arg(sendItem.treeModelItem.m_account_name)
                                                            );
        };
        const QString &key = sendItem.treeModelItem.m_protocol_name+"/"+sendItem.treeModelItem.m_parent_name;
        bool isExist = false;
        for (int i = 0;i!=m_groups.value(key,m_groups.value("unknown"))->childCount();i++) {
            if (m_groups.value(key,m_groups.value("unknown"))->child(i)->toolTip(0)==sendItem.treeModelItem.m_item_name) {
                m_groups.value(key,m_groups.value("unknown"))->child(i)->setCheckState(0,sendItem.isChecked ? Qt::Checked : Qt::Unchecked);
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            addItem(sendItem.treeModelItem,sendItem.name);
        }
    }
}


QList<SendItem> Manager::getBuddyList() const
{
    QList<SendItem> items;
    foreach (QTreeWidgetItem *group,m_groups) {
        for ( int i = 0; i != group->childCount(); i++)
        {
            SendItem item;
            item.treeModelItem = m_buddy_items.value(group->child(i)->toolTip(0))->treeModelItem;
            item.name = m_buddy_items.value(group->child(i)->toolTip(0))->name;
            item.isChecked = group->child(i)->checkState(0) == Qt::Checked;
            items << item;
        }
    }
    return items;
}
