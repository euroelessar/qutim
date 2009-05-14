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

plugDownloader::plugDownloader(const QString& path, QObject* parent)
		: QObject(parent), downloadedCount(0), totalCount(0)
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman/cache", "plugman");
	if (path.isEmpty()) {
            QFileInfo config_dir = settings.fileName();
            QDir current_dir = qApp->applicationDirPath();
            if( config_dir.canonicalPath().contains( current_dir.canonicalPath() ) )
                outPath = current_dir.relativeFilePath( config_dir.absolutePath() );
            else
                outPath = config_dir.absolutePath();
            outPath.append("/");
        }
        else
            outPath = path;
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
		emit downloadFinished(fileList);
		this->deleteLater();
		return;
	}
	downloaderItem downloadItem = m_download_queue.dequeue();

        output.setFileName(outPath+downloadItem.filename);
        if (!output.open(QIODevice::WriteOnly)) {
                qDebug() << "Unable to open file";
                startNextDownload();
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
	if (bytesTotal==0)
		return; //!hack
	qint8 value = qRound(100*bytesReceived/bytesTotal);
	m_progressBar->setValue(value);
}

void plugDownloader::downloadFinished()
{
        output.close();
	m_progressBar->reset();
	if (currentDownload->error()) {
		// download failed
		lastError= tr("Failed to download: %1").arg(output.fileName());
		qDebug() << lastError;
 	}
	else
            ++downloadedCount;
	currentDownload->deleteLater();
        if (output.exists())
            fileList.append(output.fileName()); //в случае неудачного скачивания, но наличия старой версии файла,добавляем её
	startNextDownload();
}

void plugDownloader::downloadReadyRead()
{
        output.write(currentDownload->readAll());
}
plugDownloader::~plugDownloader() {

}
void plugDownloader::setProgressbar(QProgressBar* progressBar) {
	m_progressBar = progressBar;
	m_progressBar->setFormat(tr("Downloading: %p%"));	
}

