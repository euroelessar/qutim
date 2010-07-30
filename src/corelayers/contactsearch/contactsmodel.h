/****************************************************************************
 *  contactsmodel.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include <QAbstractListModel>
#include <QPointer>
#include <QSharedPointer>
#include "libqutim/debug.h"
#include "libqutim/contactsearch.h"

namespace Core {

using namespace qutim_sdk_0_3;

typedef QSharedPointer<ContactSearchFactory> FactoryPtr;
typedef QSharedPointer<ContactSearchRequest> RequestPtr;

class ContactsModel : public QAbstractListModel
{
	Q_OBJECT
public:
	RequestPtr request() { return m_request; }
	void setRequest(const RequestPtr &request);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
private slots:
	void contactAboutToBeAdded(int row);
	void contactAdded(int row);
private:
	friend class ContactSearchForm;
	RequestPtr m_request;
};

} // namespace Core

#endif // CONTACTSMODEL_H
