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

#include "datauploader.h"

DataUploader::DataUploader(QString file, QNetworkAccessManager* transfer_manager, QObject *parent):QObject(parent){
    m_file_path = file;
    m_server = transfer_manager;
}

void DataUploader::uploadFile(){
    m_window = new ProgressWindow();
    QFileInfo file(m_file_path);
    m_window->setWindowTitle(tr("Uploading") + " " + file.baseName());
    m_window->setFileName(file.fileName());
    connect(m_window, SIGNAL(canselUpload()), this, SLOT(cancel()));
    connect(this, SIGNAL(processStarted()), m_window, SLOT(processStarted()));
    connect(this, SIGNAL(processFinished(QString)), m_window, SLOT(close()));
    m_window->show();
    makeFirstRequest();
}

DataUploader::~DataUploader(){

}

void DataUploader::makeFirstRequest(){
    QNetworkRequest request;
    request.setUrl(QUrl("http://rghost.ru/multiple/upload_host"));
    request.setRawHeader("Host","rghost.ru");
    request.setRawHeader("Connection","Keep-Alive");
    request.setRawHeader("Accept","*/*");
    connect(m_server, SIGNAL(finished(QNetworkReply*)), this, SLOT(firstRequestFinished(QNetworkReply*)));
    m_window->setStatus(tr("Preparing for file upload"));
    m_reply = m_server->get(request);
    connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), m_window, SLOT(progressChanged(qint64,qint64)));
    connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), m_window, SLOT(progressChanged(qint64,qint64)));
}

void DataUploader::firstRequestFinished(QNetworkReply* reply){
    disconnect(m_server, SIGNAL(finished(QNetworkReply*)), this, SLOT(firstRequestFinished(QNetworkReply*)));
    QString page_content(reply->readAll());
    QRegExp host("\"upload_host\":\"([^\"]*)\"");
    if (host.indexIn(page_content) != -1){
        m_upload_host = host.cap(1);
    } else {
        qDebug()<<"Error while parsing index page";
    }
    QRegExp auth_token("\"authenticity_token\":\"([^\"]*)\"");
    if (auth_token.indexIn(page_content) != -1){
        m_authenticity_token = auth_token.cap(1);
    } else {
        qDebug()<<"Error while parsing index page";
    }
    if (reply->hasRawHeader("Set-Cookie")){
        setCookie(reply->rawHeader("Set-Cookie"));
    } else{
        qDebug()<<"No cookie to set";
    }
    reply->deleteLater();
    makeSecondRequest();
}

void DataUploader::makeSecondRequest(){
    QNetworkRequest request;
    request.setUrl(QUrl("http://"+ m_upload_host + "/files"));
    request.setRawHeader("Host", m_upload_host.toLatin1());
    request.setRawHeader("Connection", "Keep-Alive");
    request.setRawHeader("Pragma", "no-cache");
    request.setRawHeader("Cache-control", "no-cache");
    request.setRawHeader("Accept", "*/*");
    QString boundary("----------u6VllBwcOfosT6RRk0TsEaquwFZPQGIIKn43z0dLdSD5YdWGdz8qXxk");
    request.setRawHeader("Content-Type", "multipart/form-data; boundary=" + boundary.toLatin1());
    RghostUploaderBuffer* data = new RghostUploaderBuffer(m_file_path, m_authenticity_token, boundary, this);
    data->open(QIODevice::ReadOnly);
    request.setRawHeader("Content-Length", QString::number(data->size()).toLatin1());
    request.setRawHeader("Cookie", m_cookie.toLatin1());
    connect(m_server, SIGNAL(finished(QNetworkReply*)), this, SLOT(secondRequestFinished(QNetworkReply*)));
    m_window->setStatus(tr("Uploading file"));
    m_reply = m_server->post(request, data);
    connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), m_window, SLOT(progressChanged(qint64,qint64)));
    connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), m_window, SLOT(progressChanged(qint64,qint64)));
    emit processStarted();
}

void DataUploader::secondRequestFinished(QNetworkReply* reply){
    disconnect(m_server, SIGNAL(finished(QNetworkReply*)), this, SLOT(secondRequestFinished(QNetworkReply*)));
    if (reply->hasRawHeader("Set-Cookie")){
        setCookie(reply->rawHeader("Set-Cookie"));
    } else{
        qDebug()<<"No cookie to set";
    }
    QString host = QString(reply->rawHeader("Location"));
    reply->deleteLater();
    makeThirdRequest(host);
}

void DataUploader::makeThirdRequest(QString host){
    QNetworkRequest request;
    request.setUrl(QUrl(host));
    request.setRawHeader("Host","rghost.ru");
    request.setRawHeader("Connection","Keep-Alive");
    request.setRawHeader("Accept","text/html");
    connect(m_server, SIGNAL(finished(QNetworkReply*)), this, SLOT(thirdRequestFinished(QNetworkReply*)));
    m_window->setStatus(tr("Getting download link"));
    m_reply = m_server->get(request);
    connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), m_window, SLOT(progressChanged(qint64,qint64)));
    connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), m_window, SLOT(progressChanged(qint64,qint64)));
}

void DataUploader::thirdRequestFinished(QNetworkReply* reply){
    disconnect(m_server, SIGNAL(finished(QNetworkReply*)), this, SLOT(thirdRequestFinished(QNetworkReply*)));
    QString page_content(reply->readAll());
    QRegExp rx("<a href=\"(http://rghost.ru/download/.+)\" class=\"file_link header_link\" title=\"Скачать\">(.+)</a>.+<span class=\"filesize\">[(](.+) (.+)[)]</span>");
    QString message;
    if (rx.indexIn(page_content) != -1){
        message = tr("You have received file ") + rx.cap(2) + " (" + rx.cap(3);
        if(rx.cap(4) == "КБ"){
            message += " Kb";
        } else if(rx.cap(4) == "MБ"){
            message += " Mb";
        } else if(rx.cap(4) == "ГБ"){
            message += " Gb";
        } else{
            message += " B";
        }
        message += ")\n" + rx.cap(1);
    } else {
        message = tr("Error while getting download link has accupied");
    }
    reply->deleteLater();
    emit processFinished(message);
}

void DataUploader::setCookie(QByteArray cookie_string){
    QRegExp rx("(_rghost_session=[^;]*);");
    QString cookie(cookie_string);
    if (rx.indexIn(cookie) != -1){
        m_cookie = rx.cap(1);
    } else{
        qDebug()<<"Error while parsing cookie string";
    }
}

void DataUploader::cancel(){
    m_window->setStatus("Canceled");
    m_reply->abort();
    m_reply->deleteLater();
    emit processFinished("!!!ERROR!!!");
}
