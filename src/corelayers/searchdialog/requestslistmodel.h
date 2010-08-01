/****************************************************************************
 *  requestslistmodel.h
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

#ifndef REQUESTSLISTMODEL_H
#define REQUESTSLISTMODEL_H

#include <QSharedPointer>
#include "libqutim/debug.h"
#include "libqutim/contactsearch.h"
#include <QAbstractListModel>

namespace Core {

using namespace qutim_sdk_0_3;

typedef AbstractSearchFactory *FactoryPtr;
typedef QSharedPointer<AbstractSearchRequest> RequestPtr;

class RequestsListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit RequestsListModel(QList<AbstractSearchFactory*> factories, QObject *parent = 0);
	virtual ~RequestsListModel();
	RequestPtr request(int row);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
	void addRequest(FactoryPtr factory, const QString &request);
	int findPlaceForRequest(FactoryPtr factory, const QString &request);
	int findRequestIndex(FactoryPtr factory, const QString &request);
private slots:
	void requestAdded(const QString &request);
	void requestRemoved(const QString &request);
	void requestUpdated(const QString &request);
private:
	QList<FactoryPtr> m_factories;
	struct RequestItem
	{
		RequestItem() :
			factory(0)
		{}
		RequestItem(const FactoryPtr &f, const QString &n) :
			factory(f), name(n)
		{}
		FactoryPtr factory;
		QString name;
	};
	QList<RequestItem> m_requestItems;
};

}

#endif // REQUESTSLISTMODEL_H
