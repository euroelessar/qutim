/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

