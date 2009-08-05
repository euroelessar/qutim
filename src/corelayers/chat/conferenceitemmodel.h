/*
    Conference Item Model

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef CONFERENCEITEMMODEL_H_
#define CONFERENCEITEMMODEL_H_

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QHash> 
#include <QList>
#include <QIcon>
#include "conferenceitem.h" 
#include "confcontactlist.h"

class ConfContactList;
class ConferenceItem;

class ConferenceItemModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	ConferenceItemModel(ConfContactList *contact_list, QObject *parent = 0);
	virtual ~ConferenceItemModel();
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
	bool addBuddy(const QString & name);	
	bool removeBuddy(const QString & name);	
	bool renameBuddy(const QString & name, const QString & new_name);
	bool setItemIcon(const QString & name, const QIcon & icon, int position);
	bool setItemRow(const QString & name, const QList<QVariant> & var, int row);
	bool setItemStatus(const QString & name, const QIcon & icon, const QString & status, int mass);
	bool setItemRole(const QString & name, const QIcon & icon, const QString & role, int mass);
	QStringList getUsers();
private:
	ConfContactList *m_contact_list;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	int position(ConferenceItem *item);
	ConferenceItem *getItem(const QModelIndex &index) const;
	QHash<QString, ConferenceItem *> m_item_hash;
	QList<ConferenceItem *> m_item_list;
	
};

#endif /*CONFERENCEITEMMODEL_H_*/
