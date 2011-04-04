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

#include <QtNetwork>
#include <QProgressBar>
#include <QTimer>


#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include "plugpackage.h"

struct downloaderItem
{
//    downloaderItem (QUrl m_url, QString m_filename);
    QUrl url; //url, с которой качаем
    QString filename; //имя файла, под которым сохраняем
	packageInfo *info; //FIXME костыль
};

class plugDownloader: public QObject
{
    Q_OBJECT
public:
    plugDownloader (const QString &path = QString::null, QObject *parent = 0);
    ~plugDownloader ();
    void startDownload();
    void addItem (const downloaderItem &downloadItem);
    QString lastError;
    QString outPath; //путь до скачанного файла
signals:
    void error(QString message);
	void downloadFinished(const QList<downloaderItem> &items);
    void updateProgressBar(const uint &completed, const uint &total, const QString &format);
private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
    void downloadReadyRead();
    void startNextDownload();

private:
    QNetworkAccessManager manager;
    QNetworkReply *currentDownload;
    QFile currentOutput;
    QTime downloadTime;
    QProgressBar *m_progressBar;
    QQueue<downloaderItem> m_download_queue;
    int downloadedCount;
    int totalCount;
	QList<downloaderItem> itemList;
	downloaderItem currentItem;
};

#endif // DOWNLOADER_H
