/*****************************************************************************
    Tree Contact List Model

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef CONTACTLISTTREEMODEL_H_
#define CONTACTLISTTREEMODEL_H_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QHash> 
#include <QIcon>
#include <QTimer>
#include <QMutex>
#include "treeitem.h" 
#include <qutim/plugininterface.h>
#include <qutim/layerinterface.h>

using namespace qutim_sdk_0_2;
class TreeItem;

class TreeContactListModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	TreeContactListModel(const QStringList &headers, QObject *parent = 0);
	~TreeContactListModel();
	void saveSettings();
	void loadSettings(const QString & profile_name);

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setHeaderData(int section, Qt::Orientation orientation,
						const QVariant &value, int role = Qt::EditRole);

	bool insertColumns(int position, int columns,
						const QModelIndex &parent = QModelIndex());
	bool removeColumns(int position, int columns,
						const QModelIndex &parent = QModelIndex());
	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());
	TreeItem *findItem(const TreeModelItem & Item);
	TreeItem *findItemNoParent(const TreeModelItem & Item);
	QModelIndex findIndex(const TreeModelItem & Item);
	QList<TreeModelItem> getItemChildren(const TreeModelItem &item = TreeModelItem());
	bool addAccount(const TreeModelItem & Item, const QString &name);
	bool addGroup(const TreeModelItem & Item, const QString &name);
	bool addBuddy(const TreeModelItem & Item, const QString &name);
	bool removeAccount(const TreeModelItem & Item);
	bool removeGroup(const TreeModelItem & Item);
	bool removeBuddy(const TreeModelItem & Item);
	void removeChildren(TreeItem *item);
	bool moveBuddy(const TreeModelItem & oldItem, const TreeModelItem & newItem);
	bool setItemName(const TreeModelItem & Item, const QString &name);
	bool setItemIcon(const TreeModelItem & Item, const QIcon &icon, int position);
	bool setItemRow(const TreeModelItem & Item, const QList<QVariant> &var, int row);
	bool setItemStatus(const TreeModelItem & Item, const QIcon &icon, const QString &status, int mass);
	void reinsertAllItems(TreeItem *item=0);
	void setItemHasUnviewedContent(const TreeModelItem & Item, bool has_content);
	bool getItemHasUnviewedContent(const TreeModelItem & item);
	bool getItemHasUnviewedContent(TreeItem *item) { return m_has_unviewed_content.value(item, false); }
	void setItemIsTyping(const TreeModelItem & Item, bool has_content);
	bool getItemIsTyping(const TreeModelItem & item);
	bool getItemIsTyping(TreeItem *item) { return m_is_typing.value(item, false); }
	void setItemIsOnline(const TreeModelItem & Item, bool online);
	void signalToDoScreenShot(int time);
	void setItemVisibility(const TreeModelItem &item, int flags);
	void setItemVisible(const TreeModelItem &item, bool visible);
	void setItemInvisible(const TreeModelItem &item, bool invisible);
public slots:
	void onTimerTimeout();
signals:
	void itemInserted(const QModelIndex &item);
	void itemRemoved(const QModelIndex &item);
	void itemNameChanged(const QModelIndex &item, const QString &value);
	void itemStatusChanged(const QModelIndex &item, const QIcon &icon, const QString &text, int mass);
	void itemVisibleChanged(const QModelIndex &item, bool visible);
	void checkItem(const QModelIndex &item);

private:
	NotificationLayerInterface *m_notification_layer;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	TreeItem *getItem(const QModelIndex &index) const;
	TreeItem m_root_item;
	QTimer *m_icon_timer;
	QString m_profile_name;
	QString m_nil_group;
	QHash<QString, QHash<QString, TreeItem *> *> m_item_list;
	QHash<TreeItem *, bool> m_has_unviewed_content;
	QHash<TreeItem *, bool> m_is_typing;
	QHash<TreeItem *, int> m_changed_status;
	QHash<TreeItem *, int> m_block_status;
	QHash<QString,QString> m_status_to_tr;
	QMutex m_mutex;
	bool m_show_special_status;
	qint32 m_time_screen_shot;
	//QHash<QString, TreeItem *> m_item_list;
	//QString toHash(const TreeModelItem & Item);
};

#endif /*TREECONTACTLISTMODEL_H_*/
