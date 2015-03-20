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
#include <qutim/contact.h>
#include <qutim/servicemanager.h>
#include <qutim/metacontactmanager.h>
#include <QSortFilterProxyModel>

class ContactListFrontModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_CLASSINFO("Service", "ContactModel")
	Q_CLASSINFO("RuntimeSwitch", "yes")
	Q_CLASSINFO("Uses", "ContactComparator")
	Q_CLASSINFO("Uses", "ContactBackendModel")
	Q_CLASSINFO("Uses", "MetaContactManager")
	Q_PROPERTY(bool showOffline READ offlineVisibility WRITE setOfflineVisibility NOTIFY offlineVisibilityChanged)
	Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
	Q_PROPERTY(QStringList filterTags READ filterTags WRITE setFilterTags NOTIFY filterTagsChanged)
public:
    explicit ContactListFrontModel(QObject *parent = 0);

	bool offlineVisibility() const;
	void setOfflineVisibility(bool showOffline);

	QStringList tags() const;
	QStringList filterTags() const;

    QHash<int, QByteArray> roleNames() const;

    virtual QStringList mimeTypes() const override;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent) override;
    virtual Qt::DropActions supportedDropActions() const override;
    virtual Qt::DropActions supportedDragActions() const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

public slots:
	void setFilterTags(const QStringList &filterTags);
	void inverseOfflineVisibility();

	void addContact(qutim_sdk_0_3::Contact *contact);
	void removeContact(qutim_sdk_0_3::Contact *contact);
    void collapse(const QModelIndex &index);
    void expand(const QModelIndex &index);
    
signals:
	void offlineVisibilityChanged(bool showOffline);
	void tagsChanged(const QStringList &tags);
	void filterTagsChanged(const QStringList &filterTags);

protected slots:
	void onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject);

protected:
    void connectNotify(const QMetaMethod &signal) override;
    void updateData(const QModelIndex &parent, int row, ContactListItemRole role);
    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onRowsRemoved(const QModelIndex &parent, int first, int last);
    bool filterAcceptsRowImpl(int sourceRow, const QModelIndex &sourceParent, bool checkCollapse) const;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

	bool m_showOffline;
    bool m_insideConnectNotify = false;
    QMetaObject::Connection m_rowsInsertedConnection;
    QMetaObject::Connection m_rowsRemovedConnection;
	QStringList m_filterTags;
	QHash<QString, QStringList> m_order;
	qutim_sdk_0_3::ServicePointer<ContactListBaseModel> m_model;
	qutim_sdk_0_3::ServicePointer<qutim_sdk_0_3::MetaContactManager> m_metaManager;
    qutim_sdk_0_3::ServicePointer<qutim_sdk_0_3::ContactComparator> m_comparator;
};

#endif // CONTACTLISTFRONTMODEL_H
