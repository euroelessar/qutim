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

#include "webkitnetworkaccessmanager.h"
#include <QNetworkRequest>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#define DEBUG() if (!(*isDebug())) {} else qDebug()
Q_GLOBAL_STATIC_WITH_ARGS(bool, isDebug, (qgetenv("ADIUM_WEBKIT_DEBUG").toInt() > 0))

WebKitNetworkAccessManager::WebKitNetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

QNetworkReply *WebKitNetworkAccessManager::createRequest(QNetworkAccessManager::Operation op,
                                                         const QNetworkRequest &request,
                                                         QIODevice *outgoingData)
{
	QNetworkRequest fixedRequest = request;
	QUrl url = request.url();
	fixLocalUrl(url);
	if (url != request.url())
		fixedRequest.setUrl(url);
	return QNetworkAccessManager::createRequest(op, fixedRequest, outgoingData);
}

void WebKitNetworkAccessManager::fixLocalUrl(QUrl &url)
{
	DEBUG() << "Try to fix" << url.toString();
	// We want to work with files in case-insensitive way
	// but we don't want to work with Windows' shared hosts (samba)
	// as they already should be case insensitive and it's rather slow
	QString fileName = url.toLocalFile();
	QString fixedFileName;
	if (!fileName.isEmpty() && !fileName.startsWith(QLatin1String("//"))) {
		// Also I suppose that fileName is always absolute path
		Q_ASSERT(QDir::isAbsolutePath(fileName));
		QDir dir;
		if (!dir.exists(fileName)) {
			QStringList paths = fileName.split(QLatin1Char('/'));
			for (int i = 0; i < paths.size(); ++i) {
				QString path = paths[i];
				path += QLatin1Char('/');
				DEBUG() << QString::fromLatin1("[%1]").arg(i) << path << "at" << dir.absolutePath();
				// First one is always absolute path
				Q_ASSERT(i > 0 || QDir::isAbsolutePath(path));
				if (!dir.exists(path)) {
					QStringList variants = dir.entryList(QStringList(paths[i]), QDir::NoFilter, QDir::Name);
					if (variants.isEmpty())
						return;
					path = variants.first() + QLatin1Char('/');
				}
				if (i == paths.size() - 1 || dir.cd(path))
					fixedFileName += path;
				else
					return;
			}
			if (fixedFileName.size() > 1 && !fileName.endsWith(QLatin1Char('/')))
				fixedFileName.chop(1);
		} else {
			fixedFileName = fileName;
		}
		DEBUG() << "Fixed request:" << url.toLocalFile() << fixedFileName;
		url = QUrl::fromLocalFile(fixedFileName);
	}
}
