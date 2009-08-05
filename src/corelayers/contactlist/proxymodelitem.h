/*****************************************************************************
    Proxy Model Item

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

#ifndef PROXYMODELITEM_H_
#define PROXYMODELITEM_H_
#include <QModelIndex>
#include <QList>
#include <QPair>
#include <QVariant>
#include "treeitem.h"

class ProxyModelItem
{
public:
	ProxyModelItem(const QModelIndex &source, QHash<TreeItem *, QModelIndex> *source_list, ProxyModelItem *parent = 0);
	virtual ~ProxyModelItem();
	ProxyModelItem *clone();
	ProxyModelItem *child(int number);
	int childCount() const;
	QVariant data(int role) const;
	bool insertChildren(int position, int count);
	ProxyModelItem *parent();
	bool removeChildren(int position, int count);
	bool removeChildren();
	bool insertChild(int position, ProxyModelItem *item);
	int childNumber() const;
	bool setData(const QVariant &value, int role);
	int indexOf(ProxyModelItem *t);
	void setSourceIndex(const QModelIndex &source);
	void setName(const QString &name);
	void moveChild(int old_position, int new_position);
	QModelIndex getSourceIndex();
	void appendSourceIndex(const QModelIndex &source);
	QList<QModelIndex> getSourceIndexes();
	int getType();
	//TreeItem *getSourceItem();
	int getMass();
	void setMass(int mass);
	QString getName();
	void setSeparator(int type, bool value);
	bool getSeparator(int type);
	bool operator >(ProxyModelItem *arg);
	void deleteItem();
	int m_online_children;
	QPair<int, QString> m_mass;
	const QList<ProxyModelItem*> *getChildren(){ return &m_child_items; }
        int getChecksum() { return m_checksum_value; }
	//TreeItem *getSourceItem();
private:
	bool m_is_deleted;
	QStringList m_child_expanded;
	QStringList m_child_order;
	QList<QModelIndex> m_source_index;
        QList<ProxyModelItem*> m_child_items;
        QList<QVariant> m_decoration;
        QList<QVariant> m_rows;
	ProxyModelItem *m_parent_item;
	QVariant m_font;
	QVariant m_color;
	//QModelIndex m_source_index;
	int m_item_type;
	int m_item_mass;
	QString m_item_name;
	QString m_item_edit;
	bool m_has_offline_separator;
	bool m_has_online_separator;
	bool m_has_nil_separator;
	QHash<TreeItem *, QModelIndex> *m_source_list;
        int m_checksum_value;
};

#endif /*PROXYMODELITEM_H_*/
