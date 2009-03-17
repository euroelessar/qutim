/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "plugdownloader.h"
#include <QSettings>
#include <QDialog>
#include <QBoxLayout>
#include <QDebug>
#include <QDir>
plugDownloader::plugDownloader(QObject* parent)
        : QObject(parent)
{

}


void plugDownloader::startDownload(const downloaderItem &downloadItem)
{

    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman/cache", "plugman");
    outPath = settings.fileName().section("/",0,-2)+"/";
    output.setFileName(outPath+downloadItem.filename);
	QDir dir;
	if (!dir.exists(outPath))
		dir.mkpath(outPath);
    if (!output.open(QIODevice::WriteOnly)) {
// 		lastError = tr("Problem opening save file '%s' for download '%s': %s\n",
// 				 qPrintable(filename), m_item.url.toEncoded().constData(),
// 				 qPrintable(output.errorString()));
        emit error(lastError);
        return;                 // skip this download
    }

    QNetworkRequest request(downloadItem.url);
    currentDownload = manager.get(request);
    connect(currentDownload, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(downloadProgress(qint64,qint64)));
    connect(currentDownload, SIGNAL(finished()),
            SLOT(downloadFinished()));
    connect(currentDownload, SIGNAL(readyRead()),
            SLOT(downloadReadyRead()));

    // prepare the output
    m_progressBar->setStatusTip(tr("Downloading %s...\n", downloadItem.url.toEncoded().constData()));
    downloadTime.start();
}

void plugDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
// 	progressBar.setStatus(bytesReceived, bytesTotal);

    // calculate the download speed
    double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
	QString unit;
	if (speed < 1024) {
		unit = "bytes/sec";
	} else if (speed < 1024*1024) {
		speed /= 1024;
		unit = "kB/s";
	} else {
		speed /= 1024*1024;
		unit = "MB/s";
	}
    qint8 value = qRound(bytesReceived/bytesTotal*100);
    m_progressBar->setValue(value);
	m_progressBar->update();
}

void plugDownloader::downloadFinished()
{
    m_progressBar->reset();
    output.close();
    if (currentDownload->error()) {
        // download failed
        lastError= tr("Failed: %s\n", qPrintable(currentDownload->errorString()));
        emit error(lastError);
    }
    else emit downloadFinished(output.fileName());
    this->deleteLater();
}

void plugDownloader::downloadReadyRead()
{
    output.write(currentDownload->readAll());
}
plugDownloader::~plugDownloader() {
    currentDownload->deleteLater();
}
void plugDownloader::setProgressbar(QProgressBar* progressBar) {
	m_progressBar = progressBar;
}

