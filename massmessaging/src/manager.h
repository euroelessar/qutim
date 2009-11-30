#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <qutim/plugininterface.h>
#include <QQueue>

using namespace qutim_sdk_0_2;

//Заведует управлением массовой рассылки

struct SendItem
{
    SendItem(const TreeModelItem &item, const QString &name);
    SendItem();
    TreeModelItem treeModelItem;
    QString name;
	bool isChecked;
};

class Manager : public QObject
{
    Q_OBJECT
public:
    Manager(QObject* parent = 0);
    ~Manager();
    QTreeWidgetItem *getRootItem();    
private:
    void createSendingQueue();
    QString parseMessage(SendItem *item);
    //QHash<QString, QTreeWidgetItem*> m_protocols; //TODO незнаю, надо или нет
    QHash<QString, QTreeWidgetItem*> m_accounts;
    QHash<QString, QTreeWidgetItem*> m_groups;
    QHash<QString, SendItem*> m_buddy_items;
    QTreeWidgetItem *m_root_item;
    QString message;
    QString title;
    uint delay;
    uint count;
    QQueue<SendItem*> m_send_items;
public slots:
    void beginSending(const QString &message,const uint &delay);
    void endSending();
	void addItem(const TreeModelItem &item,const QString &name,const bool &isChecked = false);
    void addCustomBuddyList(const QList<SendItem> &items);
	QList<SendItem> getBuddyList() const;
    void removeItem(const TreeModelItem &item);
private slots:
    void sendMessage();
signals:
    void finished(bool isSuccessfully = true);
    void updateProgressBar(const uint &completed, const uint &total, const QString &name);
};

#endif // MANAGER_H
