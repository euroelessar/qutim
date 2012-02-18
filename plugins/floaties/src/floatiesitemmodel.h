/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef FLOATIESITEMMODEL_H
#define FLOATIESITEMMODEL_H

#include <QAbstractListModel>
#include <qutim/contact.h>

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

