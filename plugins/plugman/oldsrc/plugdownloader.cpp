/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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

#include "plugdownloader.h"
#include <QSettings>
#include <QDialog>
#include <QBoxLayout>
#include <QDebug>
#include <QDir>
#include "hacks/plughacks.h"
#include <qutim/plugininterface.h>

//downloaderItem::downloaderItem(QUrl m_url, QString m_filename) {
//	url=m_url;
//	filename=m_filename;
//}
//


plugDownloader::plugDownloader(const QString& path, QObject* parent)
        : QObject(parent), downloadedCount(0), totalCount(0)
{
    outPath = path.isEmpty() ? plugPathes::getCachePath() : path;
    qDebug()<< outPath;
    QDir dir;
    dir.mkpath(outPath);
}

void plugDownloader::addItem(const downloaderItem& downloadItem)
{
    // 	if (m_download_queue.isEmpty())
    // 		QTimer::singleShot(0, this, SLOT(startNextDownload()));
    m_download_queue.enqueue(downloadItem);
    ++totalCount;

}

void plugDownloader::startDownload()
{
    startNextDownload();
}


void plugDownloader::startNextDownload()
{
    if (m_download_queue.isEmpty()) {
        emit downloadFinished(itemList);
        this->deleteLater();
        return;
    }
    currentItem =  m_download_queue.dequeue();

    currentOutput.setFileName(outPath+currentItem.filename);
    if (!currentOutput.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to open file";
        startNextDownload();
        return;                 // skip this download
    }
    QNetworkRequest request(currentItem.url);
    currentDownload = manager.get(request);
    connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(downloadProgress(qint64,qint64)));
    connect(currentDownload, SIGNAL(finished()),
            SLOT(downloadFinished()));
    connect(currentDownload, SIGNAL(readyRead()),
            SLOT(downloadReadyRead()));

    // prepare the output
    downloadTime.start();
}


void plugDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    // calculate the download speed
    double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = tr("bytes/sec");
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = tr("kB/s");
    } else {
        speed /= 1024*1024;
        unit = tr("MB/s");
    }
    if (bytesTotal==0)
        return; //!hack
    qint8 value = qRound(100*bytesReceived/bytesTotal);
    emit updateProgressBar(bytesReceived,bytesTotal,tr("Downloading: %1%, speed: %2 %3").arg(value).arg(speed).arg(unit));
}

void plugDownloader::downloadFinished()
{
    currentOutput.close();
    if (currentDownload->error()) {
        // download failed
        qDebug() << currentDownload->errorString();
        qutim_sdk_0_2::TreeModelItem item;
        //TODO может стоит сделать локализацию
        qutim_sdk_0_2::SystemsCity::PluginSystem()->systemNotification(item, currentDownload->errorString());
    }
    else {
        ++downloadedCount;
	}
    currentDownload->deleteLater();
    if (currentOutput.exists()) {
		currentItem.filename = currentOutput.fileName();
		itemList.append(currentItem);
        //fileList.append(currentOutput.fileName()); //в случае неудачного скачивания, но наличия старой версии файла,добавляем её
    }
    startNextDownload();
}

void plugDownloader::downloadReadyRead()
{
    currentOutput.write(currentDownload->readAll());
}
plugDownloader::~plugDownloader() {

}

