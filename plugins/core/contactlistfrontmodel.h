/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CONTACTLISTFRONTMODEL_H
#define CONTACTLISTFRONTMODEL_H

#include "contactlistbasemodel.h"
#include "contact.h"
#include "accountmanager.h"
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QPointer>

class ContactListFrontModel : public QSortFilterProxyModel
{
    Q_OBJECT
	Q_PROPERTY(bool showOffline READ offlineVisibility WRITE setOfflineVisibility NOTIFY offlineVisibilityChanged)
	Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
	Q_PROPERTY(QStringList filterTags READ filterTags WRITE setFilterTags NOTIFY filterTagsChanged)
public:
    explicit ContactListFrontModel(QObject *parent = 0);

	bool offlineVisibility() const;
	void setOfflineVisibility(bool showOffline);

	QStringList tags() const;
	QStringList filterTags() const;

	virtual QStringList mimeTypes() const;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
							  int row, int column, const QModelIndex &parent);
	virtual Qt::DropActions supportedDropActions() const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    QHash<int,QByteArray> roleNames() const;

public slots:
	void setFilterTags(const QStringList &filterTags);
	void inverseOfflineVisibility();

	void addContact(Contact *contact);
	void removeContact(Contact *contact);
    
signals:
	void offlineVisibilityChanged(bool showOffline);
	void tagsChanged(const QStringList &tags);
	void filterTagsChanged(const QStringList &filterTags);

protected:
    void setBaseModel(ContactListBaseModel *model);
    void onAccountsChanged(const QList<Account *> &accounts);
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    
protected:
	bool m_showOffline;
	QStringList m_filterTags;
    QList<Account *> m_accounts;
	QHash<QString, QStringList> m_order;
    QPointer<ContactListBaseModel> m_model;
    AccountManager m_manager;
};

#endif // CONTACTLISTFRONTMODEL_H
