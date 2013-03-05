/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "updater.h"
#include <qutim/config.h>
#include <qutim/settingslayer.h>
#include <qutim/debug.h>
#include <qutim/systeminfo.h>
#include <qutim/json.h>
#include <QTimerEvent>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtConcurrentFilter>
#include <QCryptographicHash>
#include <QTimer>

#define BASE_URL "http://qutim.org/client_stuff/icons"
#define ICONS_PATH "icons/hicolor/16x16/apps"

namespace Updater
{

using namespace qutim_sdk_0_3;

void UpdaterPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Updater"),
			QT_TRANSLATE_NOOP("Plugin", "In theory this plugin is auto-updater of qutIM, but currently it updates only client icons"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("euroelessar"));
}

bool UpdaterPlugin::load()
{
	m_manager.reset(new QNetworkAccessManager);
	connect(m_manager.data(), SIGNAL(finished(QNetworkReply*)), SLOT(onReplyFinished(QNetworkReply*)));
	m_watcher.reset(new QFutureWatcher<FileInfo>());
	connect(m_watcher.data(), SIGNAL(finished()), this, SLOT(onCheckFinished()));
	
	// Check for updates once per day
	m_timer.start(1000 * 60 * 60 * 24, this);
	
	updateIcons();
	return true;
}

bool UpdaterPlugin::unload()
{
	m_manager.reset(0);
	if (m_watcher->isRunning()) {
		connect(m_watcher.data(), SIGNAL(canceled()), m_watcher.data(), SLOT(deleteLater()));
		m_watcher->cancel();
		m_watcher.take();
	} else {
		m_watcher.reset(0);
	}
	m_queue.clear();
	m_timer.stop();
	return true;
}

void UpdaterPlugin::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_timer.timerId()) {
		updateIcons();
		return;
	}
	Plugin::timerEvent(event);
}

void UpdaterPlugin::updateIcons()
{
	if (m_watcher->isRunning())
		return;
	QNetworkRequest request(QUrl(QLatin1String(BASE_URL "/cache.json")));
	m_manager->get(request);
}

void UpdaterPlugin::onReplyFinished(QNetworkReply *reply)
{
	reply->deleteLater();
	const QUrl url = reply->url();
	if (url.path().endsWith(QLatin1String("cache.json"))) {
		QDir dir = SystemInfo::getDir(SystemInfo::ShareDir);
		dir.mkpath(QLatin1String(ICONS_PATH));
		if (!dir.cd(QLatin1String(ICONS_PATH)))
			return;
		QVariantMap root = Json::parse(reply->readAll()).toMap();
		QVariantList icons = root.value(QLatin1String("icons")).toList();
		FileInfo::List fileInfos;
		for (int i = 0; i < icons.size(); ++i) {
			QVariantMap icon = icons.at(i).toMap();
			FileInfo info;
			info.sha1 = icon.value(QLatin1String("sha1")).toByteArray();
			info.md5 = icon.value(QLatin1String("md5")).toByteArray();
			info.fileName = icon.value(QLatin1String("fileName")).toString();
			info.filePath = dir.filePath(info.fileName);
			fileInfos << info;
		}
		m_watcher->setFuture(QtConcurrent::filtered(fileInfos, &FileInfo::isInvalid));
	} else {
		QString filePath = reply->property("filePath").toString();
		debug() << "Received" << filePath;
		if (reply->error() == QNetworkReply::NoError) {
			QFileInfo fileInfo(filePath);
			QDir().mkpath(fileInfo.absolutePath());
			QFile file(filePath);
			if (file.open(QFile::WriteOnly)) {
				file.write(reply->readAll());
				file.close();
			}
		}
		if (m_queue.isEmpty()) {
			// Now we should force IconEngine to update icon's cache
			QDir dir = SystemInfo::getDir(SystemInfo::ShareDir);
			dir.cd(QLatin1String("icons"));
			QFile file(dir.filePath(QLatin1String("temporary-") + QString::number(qrand())));
			file.open(QFile::WriteOnly);
			file.write("123");
			file.flush();
			file.close();
			file.remove();
		} else {
			QTimer::singleShot(0, this, SLOT(requestNextUrl()));
		}
	}
}

void UpdaterPlugin::onCheckFinished()
{
	foreach (const FileInfo &info, m_watcher->future()) {
		QUrl url(QLatin1String(BASE_URL "/") + info.fileName);
		m_queue.enqueue(qMakePair(url, info.filePath));
	}
	if (!m_queue.isEmpty())
		requestNextUrl();
}

void UpdaterPlugin::requestNextUrl()
{
	if (m_queue.isEmpty())
		return;
	debug() << "Request" << m_queue.head().first;
	QNetworkRequest request(m_queue.head().first);
	m_manager->get(request)->setProperty("filePath", m_queue.head().second);
	m_queue.dequeue();
}

bool UpdaterPlugin::FileInfo::isInvalid() const
{
	QFile file(filePath);
	if (!file.open(QFile::ReadOnly))
		return true;
	QByteArray fileData = file.readAll();
	if (md5 != QCryptographicHash::hash(fileData, QCryptographicHash::Md5).toHex())
		return true;
	if (sha1 != QCryptographicHash::hash(fileData, QCryptographicHash::Sha1).toHex())
		return true;
	return false;
}

}

QUTIM_EXPORT_PLUGIN(Updater::UpdaterPlugin)

