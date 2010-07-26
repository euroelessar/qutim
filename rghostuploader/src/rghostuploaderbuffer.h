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

#ifndef RGHOSTUPLOADERBUFFER_H
#define RGHOSTUPLOADERBUFFER_H

#include <QIODevice>
#include <QBuffer>
#include <QFileInfo>
#include <QByteArray>

class RghostUploaderBuffer : public QIODevice
{
    Q_OBJECT
public:
    RghostUploaderBuffer(QString& file_name, QString& auth_token, QString& boundary, QObject *parent = 0);
    ~RghostUploaderBuffer();
    virtual bool open(OpenMode mode);
    virtual qint64 size() const;

protected:
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

private:
    QList<QIODevice *> m_devices;

};

#endif // RGHOSTUPLOADERBUFFER_H
