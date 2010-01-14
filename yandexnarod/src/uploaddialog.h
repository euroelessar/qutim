/****************************************************************************
 *  uploaddialog.h
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include <QDialog>
#include <QTime>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QBuffer>
#include <QTimer>
#include "yandexnarodauthorizator.h"
#include "ui_uploaddialog.h"
#include <qutim/contact.h>

class YandexNarodBuffer : public QIODevice
{
	Q_OBJECT
public:
	YandexNarodBuffer(const QString &fileName, const QByteArray &boundary, QObject *parent);
	~YandexNarodBuffer();

	virtual bool open(OpenMode mode);
	virtual qint64 size() const;

protected:
	virtual qint64 readData(char *data, qint64 maxlen);
	virtual qint64 writeData(const char *data, qint64 len);

private:
	QList<QIODevice *> m_devices;
};

class YandexNarodUploadDialog : public QDialog
{
	Q_OBJECT;

public:
	YandexNarodUploadDialog(QNetworkAccessManager *networkManager,
							YandexNarodAuthorizator *authorizator,
							qutim_sdk_0_3::Contact *contact = 0);
	~YandexNarodUploadDialog();

	qutim_sdk_0_3::Contact *contact() { return m_contact; }

	void start();

private:
	bool processReply(QNetworkReply *reply);

	QString m_filePath;
	QPointer<qutim_sdk_0_3::Contact> m_contact;
	QNetworkAccessManager *m_networkManager;
	YandexNarodAuthorizator *m_authorizator;
	QNetworkRequest m_request;
	QVariantMap m_someData;
	Ui::uploadDialogClass ui;
	QTimer m_timer;

signals:
	void canceled();

public slots:
	void authorizationResult(YandexNarodAuthorizator::Result, const QString &error);
	void someStrangeSlot();
	void storageReply();
	void uploadReply();
	void progressReply();

	void progress(qint64, qint64);
	void setStatus(QString str) { ui.labelStatus->setText(str); }
	void setFilename(QString str) { ui.labelFile->setText("File: "+str); this->setWindowTitle(tr("Uploading")+" - "+str); }
	void setDone() { ui.btnUploadCancel->setText(tr("Done")); }

};
#endif
