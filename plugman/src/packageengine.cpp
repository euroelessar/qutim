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

#include "packageengine.h"
#include <QNetworkReply>
#include <qutim/debug.h>
#include <qutim/systeminfo.h>
#include <attica/downloaditem.h>
#include <quasar/tar.h>
#include <quasar/zip.h>

using namespace Attica;
using namespace qutim_sdk_0_3;

PackageEngine::PackageEngine(const QStringList &categories, const QString &path)
    : m_categoriesNames(categories), m_path(path)
{
	m_path = SystemInfo::getDir(SystemInfo::ShareDir).filePath(path);
	connect(&m_manager, SIGNAL(providerAdded(Attica::Provider)), this, SLOT(onProviderAdded(Attica::Provider)));
	m_manager.loadDefaultProviders();
	m_idCounter = qrand();
}

PackageEngine::~PackageEngine()
{
}

void PackageEngine::onProviderAdded(Attica::Provider provider)
{
	m_provider = provider;
	debug() << Q_FUNC_INFO << provider.baseUrl() << provider.name();
	ListJob<Category> *job = provider.requestCategories();
	connect(job, SIGNAL(finished(Attica::BaseJob*)), this, SLOT(onCategoriesJobFinished(Attica::BaseJob*)));
	job->start();
}

void PackageEngine::onCategoriesJobFinished(Attica::BaseJob *baseJob)
{
	baseJob->deleteLater();
	ListJob<Category> *job = static_cast<ListJob<Category>*>(baseJob);
	Category::List list = job->itemList();
	for (int i = 0; i < m_categoriesNames.size(); i++) {
		for (int j = 0; j < list.size(); j++) {
			if (list.at(j).name() == m_categoriesNames.at(i))
				m_categories.append(list.at(j));
		}
	}
	m_categoriesNames.clear();
//	[23:25:57] "23" "Emoticon Theme" 
//	[23:25:57] "24" "Kopete Style 0.11" 
//	[23:25:57] "26" "Kopete Style 0.12+" 
	emit engineInitialized();
	
}

qint64 PackageEngine::requestContents(const QString &search, Attica::Provider::SortMode mode,
                                      uint page, uint pageSize)
{
	ListJob<Content> *contentJob = m_provider.searchContents(m_categories, search, mode, page, pageSize);
	connect(contentJob, SIGNAL(finished(Attica::BaseJob*)), this, SLOT(onContentJobFinished(Attica::BaseJob*)));
	qint64 id = m_idCounter++;
	contentJob->setProperty("jobId", id);
	contentJob->start();
	return id;
}

void PackageEngine::onContentJobFinished(Attica::BaseJob *baseJob)
{
	baseJob->deleteLater();
	ListJob<Content> *job = static_cast<ListJob<Content>*>(baseJob);
	Content::List list = job->itemList();
	qint64 id = job->property("jobId").toLongLong();
	debug() << Q_FUNC_INFO;
	emit contentsReceived(list, id);
}

void PackageEngine::installContent(const Attica::Content &content)
{
	ItemJob<DownloadItem> *contentJob = m_provider.downloadLink(content.id());
	connect(contentJob, SIGNAL(finished(Attica::BaseJob*)), SLOT(onDownloadJobFinished(Attica::BaseJob*)));
	contentJob->start();
}

void PackageEngine::onDownloadJobFinished(Attica::BaseJob *baseJob)
{
	baseJob->deleteLater();
	ItemJob<DownloadItem> *job = static_cast<ItemJob<DownloadItem>*>(baseJob);
	DownloadItem item = job->result();
	debug() << item.mimeType() << item.url();
	QNetworkRequest request(item.url());
	QNetworkReply *reply = m_networkManager.get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(onNetworkRequestFinished()));
}

void PackageEngine::onNetworkRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	Q_ASSERT(reply);
	QString mimeType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
	QString fileName = QDir::temp().filePath(reply->url().path().section('/', -1, -1));
	debug() << Q_FUNC_INFO;
	debug() << mimeType << fileName;
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		file.write(reply->readAll());
	}
	Quasar::Archive *archive = 0;
	
	if (mimeType == QLatin1String("application/zip")) {
        archive = new Quasar::Zip(fileName);
    } else if (mimeType == QLatin1String("application/tar")
	           || mimeType == QLatin1String("application/x-tar")
               || mimeType == QLatin1String("application/x-gzip")
               || mimeType == QLatin1String("application/x-bzip")
               || mimeType == QLatin1String("application/x-lzma")
               || mimeType == QLatin1String("application/x-xz")
               || mimeType == QLatin1String("application/x-bzip-compressed-tar")
               || mimeType == QLatin1String("application/x-compressed-tar") ) {
        archive = new Quasar::Tar(fileName);
    } else {
		warning() << "Not an archive";
		return;
    }
	archive->open(QIODevice::ReadOnly);
	archive->directory()->copyTo(m_path);
}
