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

#ifndef PACKAGEENGINE_H
#define PACKAGEENGINE_H

#include <QNetworkAccessManager>
#include <attica/content.h>
#include <attica/provider.h>
#include <attica/providermanager.h>

class PackageRequest
{
};

class PackageEngine : public QObject
{
	Q_OBJECT
public:
    PackageEngine(const QStringList &categories, const QString &path);
	~PackageEngine();

	qint64 requestContents(const QString& search = QString(),
	                       Attica::Provider::SortMode mode = Attica::Provider::Rating,
	                       uint page = 0, uint pageSize = 10);
	void installContent(const Attica::Content &content);
	
signals:
	void engineInitialized();
	void contentsReceived(const Attica::Content::List &list, qint64 id);
	
private slots:
    void onProviderAdded(Attica::Provider provider);
	void onCategoriesJobFinished(Attica::BaseJob *job);
	void onContentJobFinished(Attica::BaseJob *job);
	void onDownloadJobFinished(Attica::BaseJob *job);
	void onNetworkRequestFinished();
	
private:
	qint64 m_idCounter;
	QNetworkAccessManager m_networkManager;
	QString m_path;
	QStringList m_categoriesNames;
	Attica::Category::List m_categories;
	Attica::ProviderManager m_manager;
	Attica::Provider m_provider;
};

#endif // PACKAGEENGINE_H
