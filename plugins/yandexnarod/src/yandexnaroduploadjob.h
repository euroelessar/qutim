/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef YANDEXNARODUPLOADJOB_H
#define YANDEXNARODUPLOADJOB_H

#include "yandexnarodauthorizator.h"
#include <QDialog>
#include <QTime>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QTimer>
#include <qutim/contact.h>
#include <qutim/filetransfer.h>

using namespace qutim_sdk_0_3;
class YandexNarodFactory;

class YandexNarodBuffer : public QIODevice
{
	Q_OBJECT
public:
	YandexNarodBuffer(const QString &fileName, QIODevice *file,
					  const QByteArray &boundary, QObject *parent);
	virtual ~YandexNarodBuffer();
	virtual bool open(OpenMode mode);
	virtual qint64 size() const;
protected:
	virtual qint64 readData(char *data, qint64 maxlen);
	virtual qint64 writeData(const char *data, qint64 len);
private:
	QList<QIODevice *> m_devices;
};

class YandexNarodUploadJob : public FileTransferJob
{
	Q_OBJECT
public:
	YandexNarodUploadJob(ChatUnit *contact, YandexNarodFactory *factory);
	virtual ~YandexNarodUploadJob();
protected:
	virtual void doSend();
	virtual void doStop();
	virtual void doReceive();
public slots:
	void authorizationResult(YandexNarodAuthorizator::Result, const QString &error);
	void someStrangeSlot();
	void storageReply();
	void uploadReply();
	void progressReply();
private:
	void sendImpl();
	bool processReply(QNetworkReply *reply);
private:
#if HAS_NO_TOKEN_AUTHORIZATION
	QNetworkRequest m_request;
#endif
	QVariantMap m_someData;
	QTimer m_timer;
	QPointer<QIODevice> m_data;
};

#endif

