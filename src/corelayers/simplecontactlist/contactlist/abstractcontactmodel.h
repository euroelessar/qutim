/****************************************************************************
 *  abstractcontactmodel.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ABSTRACTCONTACTMODEL_H
#define ABSTRACTCONTACTMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include "simplecontactlist_global.h"

namespace qutim_sdk_0_3
{
class Contact;
class Account;
}

namespace Core {
namespace SimpleContactList {

class SIMPLECONTACTLIST_EXPORT AbstractContactModel : public QAbstractItemModel
{
    Q_OBJECT
	Q_CLASSINFO("Service", "ContactModel")
public:
    explicit AbstractContactModel(QObject *parent = 0);
	virtual ~AbstractContactModel();
	virtual QStringList selectedTags() const;
	virtual QStringList tags() const;
public slots:
	virtual void addContact(qutim_sdk_0_3::Contact *contact) = 0;
	virtual void removeContact(qutim_sdk_0_3::Contact *contact) = 0;
	virtual bool showOffline() const;
	virtual void hideShowOffline();
	virtual void filterList(const QString &name);
	virtual void filterList(const QStringList &tags);
signals:
	void tagVisibilityChanged(const QModelIndex &index, const QString &name, bool shown);
};

} // namespace SimpleContactList
} // namespace Core

#endif // ABSTRACTCONTACTMODEL_H
