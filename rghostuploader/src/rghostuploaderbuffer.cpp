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

#include "rghostuploaderbuffer.h"

RghostUploaderBuffer::RghostUploaderBuffer(QString& file_name, QString& auth_token, QString& boundary, QObject *parent) : QIODevice(parent){
    QFileInfo info(file_name);
    QByteArray data;
    data.append("--" + boundary + "\r\n");
    data.append("Content-Disposition: form-data; name=\"authenticity_token\"\r\n");
    data.append("\r\n");
    data.append(auth_token + "\r\n");
    data.append("--" + boundary + "\r\n");
    data.append("Content-Disposition: form-data; name=\"file\"; filename=\"" + info.fileName().toUtf8() + "\"\r\n");
    data.append("Content-Type: application\r\n");
    data.append("Content-Transfer-Encoding: binary\r\n");
    data.append("\r\n");
    QBuffer *buffer = new QBuffer(this);
    buffer->setData(data);
    m_devices.append(buffer);
    m_devices.append(new QFile(file_name, this));
    data.clear();
    data.append("\r\n--" + boundary + "\r\n");
    data.append("Content-Disposition: form-data; name=\"commit\"");
    data.append("\r\n");
    data.append("\r\n..................");
    data.append("\r\n--" + boundary + "--\r\n");
    buffer = new QBuffer(this);
    buffer->setData(data);
    m_devices.append(buffer);
}

RghostUploaderBuffer::~RghostUploaderBuffer(){

}

bool RghostUploaderBuffer::open(OpenMode mode){
    bool result = true;
    foreach (QIODevice *device, m_devices)
        result &= device->open(mode);
    QIODevice::open(mode);
    return result;
}

qint64 RghostUploaderBuffer::size() const{
    quint64 totalSize = 0;
    foreach (QIODevice *device, m_devices)
        totalSize += device->size();
    return totalSize;
}

qint64 RghostUploaderBuffer::readData(char *data, qint64 maxlen){
    qint64 totalSize = 0;
    foreach (QIODevice *device, m_devices) {
        if (maxlen == 0 || device->atEnd())
                continue;
        qint64 currentSize = device->read(data, maxlen);
        totalSize += currentSize;
        maxlen -= currentSize;
        data += currentSize;
    }
    return totalSize;
}

qint64 RghostUploaderBuffer::writeData(const char *data, qint64 len){
    Q_UNUSED(data);
    Q_UNUSED(len);
    return -1;
}
