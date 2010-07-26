/**************************************************************************
*  Rghost Uploader. qutIM 0.3 plugin, that allows users to send files via *
*  rghost.ru serviсe.                                                     *
*  Copyright (C) 2010  Ian Kazlauskas <nayzak90@googlemail.com>           *
*                                                                         *
*  This library is free software: you can redistribute it and/or modify   *
*  it under the terms of the GNU General Public License as published by   *
*  the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                    *
*                                                                         *
*  This library is distributed in the hope that it will be useful,        *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*  GNU General Public License for more details.                           *
*                                                                         *
*  You should have received a copy of the GNU General Public License      *
*  along with this library.  If not, see <http://www.gnu.org/licenses/>.  *
***************************************************************************/

#ifndef DATAUPLOADER_H
#define DATAUPLOADER_H

#include <progresswindow.h>
#include <rghostuploaderbuffer.h>
#include <QObject>
#include <QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class DataUploader : public QObject{
Q_OBJECT
public:
    DataUploader(QString file, QNetworkAccessManager* transfer_manager, QObject *parent = 0);
    ~DataUploader();
    void uploadFile();
private:
    QString m_file_path;
    QString m_cookie;
    QString m_upload_host;
    QString m_authenticity_token;
    QNetworkAccessManager* m_server;
    QNetworkReply* m_reply;
    ProgressWindow* m_window;
    void makeFirstRequest();
    void makeSecondRequest();
    void makeThirdRequest(QString host);
    void setCookie(QByteArray cookie_string);
signals:
    void processFinished(QString message);
    void processStarted();
public slots:
    void cancel();
private slots:
    void firstRequestFinished(QNetworkReply*);
    void secondRequestFinished(QNetworkReply*);
    void thirdRequestFinished(QNetworkReply*);
};

#endif // DATAUPLOADER_H
