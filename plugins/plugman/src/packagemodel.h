/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
	Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
	Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
	Q_PROPERTY(QStringList categories READ categories WRITE setCategories NOTIFY categoriesChanged)
	Q_PROPERTY(SortMode sortMode READ sortMode WRITE setSortMode NOTIFY sortModeChanged)
	Q_PROPERTY(bool loading READ isLoading NOTIFY isLoadingChanged)
	Q_ENUMS(SortMode)
	Q_ENUMS(Status)
public:
	enum SortMode
	{
		Newest = Attica::Provider::Newest,
		Alphabetical = Attica::Provider::Alphabetical,
		Rating = Attica::Provider::Rating,
		Downloads = Attica::Provider::Downloads
	};
	enum Status
	{
		Invalid = PackageEntry::Invalid,
		Installable = PackageEntry::Installable,
		Updateable = PackageEntry::Updateable,
		Installing = PackageEntry::Installing,
		Updating = PackageEntry::Updating,
		Installed = PackageEntry::Installed
	};

	explicit PackageModel(QObject *parent = 0);
	
	void setFilter(const QString &filter);
	QString filter() const;
	void setSortMode(SortMode mode);
	SortMode sortMode() const;
	void setPath(const QString &path);
	QString path() const;
	void setCategories(const QStringList &categories);
	QStringList categories() const;
	bool isLoading() const;

	PackageEngine *engine() const;

	void reset();
	
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
public slots:
	void requestNextPage();
	void remove(int index);
	void install(int index);

signals:
	void filterChanged(const QString &filter);
	void sortModeChanged(SortMode sortMode);
	void pathChanged(const QString &path);
	void categoriesChanged(const QStringList &categories);
	void isLoadingChanged(bool);
	
private slots:
	void onContentsReceived(const PackageEntry::List &list, qint64 id);
	void onEntryChanged(const QString &id);

protected:
	void setIsLoading(bool isLoading);
	
private:
	PackageEngine *m_engine;
	QString m_filter;
	QString m_path;
	QStringList m_categories;
	SortMode m_mode;
	QHash<QString, int> m_indexes;
	PackageEntry::List m_contents;
	int m_pageSize;
	int m_pagesCount;
	qint64 m_requestId;
	bool m_isLoading;
};

#endif // PACKAGEMODEL_H

