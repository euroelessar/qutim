/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include <QAbstractListModel>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/contact.h>

namespace MeegoIntegration
{
class ContactListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ContactListModel();
	virtual ~ContactListModel();
	
	// QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
	
signals:
	
private slots:
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountDeath(QObject *object);
	void onContactCreated(qutim_sdk_0_3::Contact *contact);
	void onContactTitleChanged(const QString &title, const QString &oldTitle);
	void onContactStatusChanged(const qutim_sdk_0_3::Status &status);
	void onContactDeath(QObject *object);
	
private:
	struct Item {
		QString title;
		qutim_sdk_0_3::Contact *contact;
		
		bool operator <(const Item &o) const;
	};
	QHash<QObject*, QString> m_titles;
	QList<Item> m_contacts;
};
}

#endif // CONTACTLISTMODEL_H
