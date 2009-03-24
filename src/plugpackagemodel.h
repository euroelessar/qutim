/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#ifndef PLUGPACKAGEMODEL_H
#define PLUGPACKAGEMODEL_H

#include <QAbstractItemModel>
#include "plugpackage.h"
#include "plugpackageitem.h"

class PlugPackageModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	PlugPackageModel(QObject *parent);
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return 1; }
	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	void addItem(const packageInfo& item);
	void removeItem(const packageInfo &item);
	void moveItem(const packageInfo & old_item, const packageInfo & new_item);
	void setItemName(const packageInfo &item, const QString & name);
	void setItemIcon(const packageInfo &item, const QIcon &icon, int position);
	void setItemShortDesc(const packageInfo &item,const QString & shortdesc);
	void setItemDescription(const packageInfo &item, const QVector<QVariant> &text);
	void setItemVisibility(const packageInfo &item, int flags);
	void setItemAttribute(const packageInfo &item, packageAttribute type, bool on);
	QStringList getItemChildren(const packageInfo &item);
private:
	void addItem(const packageInfo &item, plugPackageItem *data_item);
	inline plugPackageItem *getItem(const QModelIndex &index) const;
	plugPackageItem *getItem(const packageInfo &item) const;
	QHash<QString, QHash<QString, plugPackageItem *> > m_items; // protocols and accounts
	plugPackageItem *m_root_item;
};

#endif // PLUGPACKAGEMODEL_H
