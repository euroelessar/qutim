/****************************************************************************
 *  model.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef MODEL_H
#define MODEL_H

#include <QStandardItemModel>
#include <QPointer>

namespace qutim_sdk_0_3 {
class MetaContact;
class Contact;
}

namespace Core {
namespace MetaContacts {

class MetaContactImpl;
class Model : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = 0);
	void setMetaContact(MetaContactImpl*);
	MetaContactImpl *metaContact() const;
public slots:
	void searchContacts(const QString &name);
	void activated(const QModelIndex &index);
signals:
	void addContactTriggered(qutim_sdk_0_3::Contact*);
	void removeContactTriggered(qutim_sdk_0_3::Contact*);
private:
	void addContact(qutim_sdk_0_3::Contact *,QStandardItem *root);
	QPointer<MetaContactImpl> m_metaContact;
	QStandardItem *m_metaRoot;
	QStandardItem *m_searchRoot;
};

} // namespace MetaContacts
} // namespace Core

#endif // MODEL_H
