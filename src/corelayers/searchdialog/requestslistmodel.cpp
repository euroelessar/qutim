/****************************************************************************
 *  requestslistmodel.cpp
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

#include "requestslistmodel.h"
#include <libqutim/objectgenerator.h>

namespace Core {

RequestsListModel::RequestsListModel(QList<AbstractSearchFactory*> factories, QObject *parent) :
	QAbstractListModel(parent), m_factories(factories)
{
	foreach (AbstractSearchFactory *factory, factories) {
		connect(factory, SIGNAL(requestAdded(QString)),
				SLOT(requestAdded(QString)));
		connect(factory, SIGNAL(requestRemoved(QString)),
				SLOT(requestRemoved(QString)));
		connect(factory, SIGNAL(requestUpdated(QString)),
				SLOT(requestUpdated(QString)));
		foreach (const QString &request, factory->requestList())
			addRequest(factory, request);
	}
}

RequestsListModel::~RequestsListModel()
{
}

RequestPtr RequestsListModel::request(int row)
{
	RequestItem item = m_requestItems.value(row);
	if (item.factory)
		return RequestPtr(item.factory->request(item.name));
	return RequestPtr();
}

int RequestsListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_requestItems.count();
}

QVariant RequestsListModel::data(const QModelIndex &index, int role) const
{
	if (index.column() != 0 || index.row() < 0 || index.row() >= m_requestItems.count())
		return QVariant();
	const RequestItem &item = m_requestItems.at(index.row());
	return item.factory->data(item.name, role);
}

void RequestsListModel::addRequest(FactoryPtr factory, const QString &request)
{
	// Add the request item.
	int place = findPlaceForRequest(factory, request);
	beginInsertRows(QModelIndex(), place, place);
	m_requestItems.insert(place, RequestItem(factory, request));
	endInsertRows();
}

int RequestsListModel::findPlaceForRequest(FactoryPtr factory, const QString &request)
{
	bool factoryFound = false;
	int count = m_requestItems.count();
	QString requestTitle;
	for (int i = 0; i < count; ++i) {
		const RequestItem &item = m_requestItems.at(i);
		if (!factoryFound) {
			// First, we need to find the factory in the items list.
			if (item.factory == factory) {
				factoryFound = true;
				requestTitle = factory->data(request).toString();
			} else {
				continue;
			}
		}
		// The factory has been found, hence we can look for a suitable place for our item.
		if (item.factory == factory) {
			if (item.name == request)
				return i;
			QString itemTitle = item.factory->data(item.name).toString();
			if (QString::localeAwareCompare(itemTitle, requestTitle) > 0)
				return i;
		} else {
			return i;
		}
	}
	return count;
}

int RequestsListModel::findRequestIndex(FactoryPtr factory, const QString &request)
{
	for (int i = 0, c = m_requestItems.count(); i < c; ++i) {
		const RequestItem &item = m_requestItems.at(i);
		if (item.factory == factory && item.name == request)
			// Well. We found it.
			return i;
	}
	return -1;
}

void RequestsListModel::requestAdded(const QString &request)
{
	Q_ASSERT(qobject_cast<FactoryPtr>(sender()));
	// A factory the request has been added to.
	FactoryPtr factory = reinterpret_cast<FactoryPtr>(sender());
	addRequest(factory, request);
}

void RequestsListModel::requestRemoved(const QString &request)
{
	Q_ASSERT(qobject_cast<FactoryPtr>(sender()));
	// A factory the request has been removed from.
	FactoryPtr factory = reinterpret_cast<FactoryPtr>(sender());
	// Find the item and remove it.
	int pos = findRequestIndex(factory, request);
	if (pos != -1) {
		beginRemoveRows(QModelIndex(), pos, pos);
		m_requestItems.removeAt(pos);
		endRemoveRows();
	}
}

void RequestsListModel::requestUpdated(const QString &request)
{
	Q_ASSERT(qobject_cast<FactoryPtr>(sender()));
	FactoryPtr factory = reinterpret_cast<FactoryPtr>(sender());
	int oldPos = findRequestIndex(factory, request);
	if (oldPos != -1) {
		int newPos = findPlaceForRequest(factory, request);
		if (oldPos == newPos) {
			QModelIndex i = index(oldPos, 0);
			emit dataChanged(i, i);
		} else {
			beginMoveRows(QModelIndex(), oldPos, oldPos, QModelIndex(), newPos);
			m_requestItems.insert(newPos, m_requestItems.takeAt(oldPos));
			endMoveRows();
		}
	}
}

}
