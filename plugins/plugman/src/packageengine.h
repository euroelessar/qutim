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

#ifndef PACKAGEENGINE_H
#define PACKAGEENGINE_H

#include <QNetworkAccessManager>
#include <attica/content.h>
#include <attica/provider.h>
#include <attica/providermanager.h>
#include "packageentry.h"
#include <QStringList>

class PackageEngine : public QObject
{
	Q_OBJECT
public:
	PackageEngine(QObject *parent = 0);
	~PackageEngine();
	
	bool isInitialized();
	qint64 requestContents(const Attica::Category::List &categories = Attica::Category::List(),
						   const QString& search = QString(),
	                       Attica::Provider::SortMode mode = Attica::Provider::Rating,
	                       uint page = 0, uint pageSize = 10);
	Attica::Category::List resolveCategories(const QStringList &categoriesNames) const;
	void remove(const PackageEntry &entry);
	void install(const PackageEntry &entry, const QString &path);
	void loadPreview(const PackageEntry &entry);
	
signals:
	void engineInitialized();
	void contentsReceived(const PackageEntry::List &list, qint64 id);
	void previewLoaded(const QString &id, const QPixmap &preview);
	void entryChanged(const QString &id);
	
private slots:
    void onProviderAdded(Attica::Provider provider);
	void onCategoriesJobFinished(Attica::BaseJob *job);
	void onContentJobFinished(Attica::BaseJob *job);
	void onDownloadJobFinished(Attica::BaseJob *job);
	void onPreviewRequestFinished();
	void onNetworkRequestFinished();
	
private:
	qint64 m_idCounter;
	Attica::Category::List m_categories;
	QNetworkAccessManager m_networkManager;
	QHash<QString, PackageEntry> m_entries;
	Attica::ProviderManager m_manager;
	Attica::Provider m_provider;
};

#endif // PACKAGEENGINE_H

