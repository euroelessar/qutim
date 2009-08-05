/*****************************************************************************
    Contact List Proxy Model

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

#ifndef CONTACTLISTPROXYMODEL_H_
#define CONTACTLISTPROXYMODEL_H_
#include <QAbstractProxyModel>
#include <QHash>
#include <QTreeView>
#include <QStringList>
#include <QLinkedList>
#include "treecontactlistmodel.h"
#include "proxymodelitem.h"

using namespace qutim_sdk_0_2;

class ContactListProxyModel : public QAbstractProxyModel
{
	Q_OBJECT
public:
	ContactListProxyModel( QObject * parent = 0 );
	virtual ~ContactListProxyModel(); 
	QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
	QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool insertColumns(int position, int columns,
						const QModelIndex &parent = QModelIndex());
	bool removeColumns(int position, int columns,
						const QModelIndex &parent = QModelIndex());
	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());
	void setModel(TreeContactListModel *model);
	void setTreeView(QTreeView *tree_view);
	int findPosition(const QModelIndex &parent, const QString &name, int mass=-100);
	void removeAllItems();
	void setShowOffline(bool show);
	void setShowEmptyGroup(bool show);
	void setSortStatus(bool sort);
	void setShowSeparator(bool show);
	void setModelType(int type);
	bool getShowOffline();
	bool getShowEmptyGroup();
	bool getSortStatus();
	bool getShowSeparator();
	int getModelType();
	void setSettings(int type, bool show_offline, bool show_empty, bool sort_status, bool show_separator, const QVariant & account_font, const QVariant & group_font, const QVariant & online_font, const QVariant & offline_font, const QVariant & separator_font, const QVariant & account_color, const QVariant & group_color, const QVariant & online_color, const QVariant & offline_color, const QVariant & separator_color);
	void loadProfile(const QString & profile_name);
	void saveSettings();
	void setExpanded(const QModelIndex & index, bool expanded);
private:
	int getChildPosition(const QString &child);
	void moveChild(const QString &child, int position);
	void setChildOrder(const QStringList &order);
	QStringList getChildOrder();
	void setExpanded(const QString & child, bool expanded);
	bool isExpanded(const QString & child);
	QString m_profile_name;
	QStringList m_is_expanded;
	QStringList m_child_order;
	ProxyModelItem *getItem(const QModelIndex &index) const;
	ProxyModelItem *m_root_item;
	TreeContactListModel *m_tree_model;
	QHash<TreeItem *, QModelIndex> m_source_list;
	QTreeView *m_tree_view;
	int m_model_type;
	bool m_show_offline;
	bool m_show_empty_group;
	bool m_sort_status;
	bool m_show_separator;
	QVariant m_account_font;
	QVariant m_group_font;
	QVariant m_online_font;
	QVariant m_offline_font;
	QVariant m_separator_font;
	QVariant m_account_color;
	QVariant m_group_color;
	QVariant m_online_color;
	QVariant m_offline_color;
	QVariant m_separator_color;
	QModelIndex createIndex_(ProxyModelItem *item);
	ProxyModelItem* m_position_buddy;
	QAbstractItemModel *m_empty_model;
	QLinkedList<ProxyModelItem *> m_list_for_expand;
	bool m_append_to_expand_list;
private slots:
	void collapsed( const QModelIndex &index );
	void expanded( const QModelIndex &index );
public slots:
	void insertItem(const QModelIndex &source);
	void removeItem(const QModelIndex &source);
	void setName(const QModelIndex &source, const QString &value);
	void setStatus(const QModelIndex &source, const QIcon &icon, const QString &text, int mass);
	void newData(const QModelIndex &left, const QModelIndex &right);
	void checkItem(const QModelIndex &source);
};

#endif /*CONTACTLISTPROXYMODEL_H_*/
