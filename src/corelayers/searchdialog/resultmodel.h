/****************************************************************************
 *  resultmodel.h
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

#ifndef RESULTMODEL_H
#define RESULTMODEL_H

#include <QAbstractListModel>
#include <QPointer>
#include "requestslistmodel.h"

namespace Core {

class ResultModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ResultModel(QObject *parent = 0);
	RequestPtr request() { return m_request; }
	void setRequest(const RequestPtr &request);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
signals:
	void rowAdded(int row);
private slots:
	void onRowAboutToBeAdded(int row);
	void onRowAdded(int row);
private:
	friend class AbstractSearchForm;
	RequestPtr m_request;
};

} // namespace Core

#endif // RESULTMODEL_H
