/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef PACKAGEMODEL_H
#define PACKAGEMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include "packageengine.h"

struct PackageItem
{
	Attica::Content content;
	QPixmap preview;
};

class PackageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PackageModel(PackageEngine *engine);
	
	void setFilter(const QString &filter);
	void setSortMode(Attica::Provider::SortMode mode);
	
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
public slots:
	void requestNextPage();
	
private slots:
	void onContentsReceived(const PackageEntry::List &list, qint64 id);
	void onPreviewLoaded(const QString &id, const QPixmap &preview);
	void onEntryChanged(const QString &id);
	
private:
	PackageEngine *m_engine;
	QString m_filter;
	Attica::Provider::SortMode m_mode;
	QHash<QString, int> m_indexes;
	PackageEntry::List m_contents;
	int m_pageSize;
	int m_pagesCount;
	qint64 m_requestId;
};

#endif // PACKAGEMODEL_H
