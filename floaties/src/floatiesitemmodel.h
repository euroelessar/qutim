/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef FLOATIESITEMMODEL_H
#define FLOATIESITEMMODEL_H

#include <QAbstractListModel>
#include <qutim/contact.h>
//FIXME
//#include <contactdelegate.h>

class FloatiesItemModel : public QAbstractListModel
{
	Q_OBJECT
public:
	// Keep in sync with core's contact list
	enum ContactItemType
	{
		InvalidType = 0,
		TagType = 100,
		ContactType = 101
	};

	enum ContactItemRole
	{
		BuddyRole = Qt::UserRole,
		StatusRole,
		ContactsCountRole,
		OnlineContactsCountRole,
		AvatarRole,
		ItemTypeRole
	};

	FloatiesItemModel(QObject *parent);
	~FloatiesItemModel();
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	QPersistentModelIndex addContact(qutim_sdk_0_3::Contact *contact);
	void removeContact(qutim_sdk_0_3::Contact *contact);
private:
	QList<qutim_sdk_0_3::Contact*> m_contacts;
};

#endif // FLOATIESITEMMODEL_H
