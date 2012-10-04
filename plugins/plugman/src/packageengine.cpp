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

#include "packageengine.h"
#include "plugmanarchive.h"
#include <QNetworkReply>
#include <QPixmap>
#include <QDirIterator>
#include <qutim/debug.h>
#include <qutim/systeminfo.h>
#include <qutim/jsonfile.h>
#include <attica/downloaditem.h>

using namespace Attica;
using namespace qutim_sdk_0_3;

PackageEngine::PackageEngine(QObject *parent)
	: QObject(parent)
{
	connect(&m_manager, SIGNAL(providerAdded(Attica::Provider)),
			this, SLOT(onProviderAdded(Attica::Provider)));
	if (m_manager.defaultProviderFiles().isEmpty()) {
		m_manager.addProviderFile(QUrl(QLatin1String("http://download.kde.org/ocs/providers.xml")));
		m_manager.addProviderFile(QUrl(QLatin1String("http://qutim.org/ocs/providers.xml")));
	} else {
		m_manager.loadDefaultProviders();
	}
	m_idCounter = (qint64(qrand()) << 32) | quint32(qrand());

	const QString fileName = SystemInfo::getDir(SystemInfo::ShareDir)
							 .filePath(QLatin1String("packages.json"));
	JsonFile file(fileName);
	QVariant var;
	if (file.load(var)) {
		foreach (var, var.toList()) {
			QVariantMap data = var.toMap();
			Attica::Content content;
			PackageEntry entry;
			content.setId(data.value(QLatin1String("id")).toString());
			content.setName(data.value(QLatin1String("name")).toString());
			content.setRating(data.value(QLatin1String("rating")).toInt());
			content.setDownloads(data.value(QLatin1String("downloads")).toInt());
			content.setCreated(data.value(QLatin1String("created")).toDateTime());
			content.setUpdated(data.value(QLatin1String("updated")).toDateTime());
			QMapIterator<QString, QVariant> it(data.value(QLatin1String("attributes")).toMap());
			while (it.hasNext()) {
				it.next();
				content.addAttribute(it.key(), it.value().toString());
			}
			entry.setContent(content);
			entry.setInstalledFiles(data.value(QLatin1String("installedFiles")).toStringList());
			entry.setInstalledVersion(data.value(QLatin1String("installedVersion")).toString());
			QString status = data.value(QLatin1String("status")).toString();
			if (status == QLatin1String("updateable"))
				entry.setStatus(PackageEntry::Updateable);
			else
				entry.setStatus(PackageEntry::Installed);
			m_entries.insert(entry.id(), entry);
		}
	}
}

PackageEngine::~PackageEngine()
{
	QVariantList entries;
	foreach (const PackageEntry &entry, m_entries) {
		if (entry.status() != PackageEntry::Installed
		        && entry.status() != PackageEntry::Updating
		        && entry.status() != PackageEntry::Updateable) {
			continue;
		}
		QVariantMap data;
		data.insert(QLatin1String("id"), entry.id());
		data.insert(QLatin1String("name"), entry.content().name());
		data.insert(QLatin1String("rating"), entry.content().rating());
		data.insert(QLatin1String("downloads"), entry.content().downloads());
		data.insert(QLatin1String("created"), entry.content().created().toString(Qt::ISODate));
		data.insert(QLatin1String("updated"), entry.content().updated().toString(Qt::ISODate));
		QVariantMap attributes;
		QMapIterator<QString, QString> it(entry.content().attributes());
		while (it.hasNext()) {
			it.next();
			attributes.insert(it.key(), it.value());
		}
		data.insert(QLatin1String("attributes"), attributes);
		data.insert(QLatin1String("installedFiles"), entry.installedFiles());
		data.insert(QLatin1String("installedVersion"), entry.installedVersion());
		QString status;
		if (entry.status() == PackageEntry::Installed)
			status = QLatin1String("installed");
		else
			status = QLatin1String("updateable");
		data.insert(QLatin1String("status"), status);
		entries.append(data);
	}

	const QString fileName = SystemInfo::getDir(SystemInfo::ShareDir)
							 .filePath(QLatin1String("packages.json"));
	JsonFile file(fileName);
	file.save(entries);
}

bool PackageEngine::isInitialized()
{
	return !m_categories.isEmpty();
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
	m_categories = job->itemList();
	for (int j = 0; j < m_categories.size(); j++) {
		debug() << j << m_categories[j].name();
	}
//	[23:25:57] "23" "Emoticon Theme" 
//	[23:25:57] "24" "Kopete Style 0.11" 
//	[23:25:57] "26" "Kopete Style 0.12+" 
	emit engineInitialized();
	
}

qint64 PackageEngine::requestContents(const Attica::Category::List &categories, const QString &search,
									  Attica::Provider::SortMode mode, uint page, uint pageSize)
{
	ListJob<Content> *contentJob = m_provider.searchContents(categories, search, mode, page, pageSize);
	connect(contentJob, SIGNAL(finished(Attica::BaseJob*)), this, SLOT(onContentJobFinished(Attica::BaseJob*)));
	qint64 id = m_idCounter++;
	contentJob->setProperty("jobId", id);
	contentJob->start();
	return id;
}

Category::List PackageEngine::resolveCategories(const QStringList &categoriesNames) const
{
	Category::List categories;
	for (int i = 0; i < categoriesNames.size(); i++) {
		for (int j = 0; j < m_categories.size(); j++) {
			if (m_categories.at(j).name() == categoriesNames.at(i))
				categories.append(m_categories.at(j));
		}
	}
	return categories;
}

void PackageEngine::onContentJobFinished(Attica::BaseJob *baseJob)
{
	baseJob->deleteLater();
	ListJob<Content> *job = static_cast<ListJob<Content>*>(baseJob);
	Content::List contents = job->itemList();
	PackageEntry::List list;
	for (int i = 0; i < contents.size(); ++i) {
		const Content &content = contents.at(i);
		PackageEntry &entry = m_entries[content.id()];
		entry.setContent(content);
		if (entry.status() == PackageEntry::Installed && entry.installedVersion() != content.version()) {
			entry.setStatus(PackageEntry::Updateable);
			emit entryChanged(entry.id());
		} else if (entry.status() == PackageEntry::Invalid) {
			entry.setStatus(PackageEntry::Installable);
			emit entryChanged(entry.id());
		}
		list.append(entry);
	}
	qint64 id = job->property("jobId").toLongLong();
	debug() << Q_FUNC_INFO;
	emit contentsReceived(list, id);
}

void PackageEngine::remove(const PackageEntry &entry)
{
	PackageEntry entryHook = entry;
	const QDir dir;
	const QStringList files = entry.installedFiles();
	for (int i = files.size() - 1; i >= 0; --i) {
		if (files.at(i).endsWith('/'))
			dir.rmpath(files.at(i));
		else
			QFile::remove(files.at(i));
	}
	entryHook.setInstalledFiles(QStringList());
	entryHook.setInstalledVersion(QString());
	entryHook.setStatus(PackageEntry::Installable);
	emit entryChanged(entry.id());
}

void PackageEngine::install(const PackageEntry &entry, const QString &path)
{
	PackageEntry entryHook = entry;
	Attica::Content content = entry.content();
	ItemJob<DownloadItem> *contentJob = m_provider.downloadLink(content.id());
	contentJob->setProperty("contentId", entry.id());
	contentJob->setProperty("path", path);
	connect(contentJob, SIGNAL(finished(Attica::BaseJob*)), SLOT(onDownloadJobFinished(Attica::BaseJob*)));
	contentJob->start();
	if (entry.status() == PackageEntry::Installed)
		entryHook.setStatus(PackageEntry::Updating);
	else
		entryHook.setStatus(PackageEntry::Installing);
	emit entryChanged(entry.id());
}

void PackageEngine::loadPreview(const PackageEntry &entry)
{
	Attica::Content content = entry.content();
	QNetworkRequest request(QUrl::fromUserInput(content.smallPreviewPicture()));
	QNetworkReply *reply = m_networkManager.get(request);
	reply->setProperty("contentId", content.id());
	connect(reply, SIGNAL(finished()), SLOT(onPreviewRequestFinished()));
}

void PackageEngine::onDownloadJobFinished(Attica::BaseJob *baseJob)
{
	baseJob->deleteLater();
	ItemJob<DownloadItem> *job = static_cast<ItemJob<DownloadItem>*>(baseJob);
	DownloadItem item = job->result();
	debug() << item.url();
	QNetworkRequest request(item.url());
	QNetworkReply *reply = m_networkManager.get(request);
	reply->setProperty("path", job->property("path"));
	reply->setProperty("contentId", job->property("contentId"));
	connect(reply, SIGNAL(finished()), this, SLOT(onNetworkRequestFinished()));
}

void PackageEngine::onPreviewRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	Q_ASSERT(reply);
	QString id = reply->property("contentId").toString();
	QPixmap pixmap = QPixmap::fromImage(QImage::fromData(reply->readAll()));
	emit previewLoaded(id, pixmap);
}

void PackageEngine::onNetworkRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	Q_ASSERT(reply);
	QString mimeType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
	QString fileName = QDir::temp().filePath(reply->url().path().section('/', -1, -1));
	PackageEntry entry = m_entries[reply->property("contentId").toString()];
	debug() << Q_FUNC_INFO;
	debug() << mimeType << fileName;
	{
		QFile file(fileName);
		file.open(QIODevice::WriteOnly);
		file.write(reply->readAll());
	}
	
	QDir tmp = QDir::temp();
	QString subpath = QLatin1String("qutim-plugman-") + QString::number(qrand());
	if (!tmp.mkdir(subpath)) {
		critical() << "Can't create subdirectory at temporary path" << tmp;
		return;
	}
	if (!tmp.cd(subpath)) {
		critical() << "Can't enter subdirectory at temporary path" << tmp;
		return;
	}
	QString error;
	if (!PlugmanArchive::extract(fileName, tmp.absolutePath(), &error)) {
		critical() << "Can't extract archive" << fileName << "to" << subpath << "(" + error + ")";
		return;
	}
	bool before = blockSignals(true);
	remove(entry);
	blockSignals(before);
	QStringList files;
	QString path = reply->property("path").toString();
	QString fullPath = SystemInfo::getDir(SystemInfo::ShareDir).filePath(path);
	QDir dir = fullPath;
	QList<QString> dirsToRemove;
	dirsToRemove << tmp.absolutePath();
	QDirIterator it(tmp.absolutePath(), QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString fileName = tmp.relativeFilePath(it.next());
		QString filePath = dir.absoluteFilePath(fileName);
		debug() << fileName << filePath;
		QFileInfo info = it.fileInfo();
		if (info.isDir()) {
			if (!dir.exists(fileName))
				dir.mkpath(fileName);
			dirsToRemove << filePath;
			files << (filePath + '/');
		} else {
			if (QFile::rename(info.absoluteFilePath(), filePath)) {
				files << filePath;
			} else {
				warning() << "Can't move file" << info.absoluteFilePath() << "to" << filePath;
				QFile::remove(info.absoluteFilePath());
			}
		}
	}
	tmp.cdUp();
	while (!dirsToRemove.isEmpty())
		tmp.rmpath(dirsToRemove.takeLast());
	entry.setStatus(PackageEntry::Installed);
	entry.setInstalledFiles(files);
	entry.setInstalledVersion(entry.content().version());
	QFile::remove(fileName);
	emit entryChanged(entry.id());
}

