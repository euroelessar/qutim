/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexander Kazarin <boiler@co.ru>
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

#ifndef YANDEXNARODNETMAN_H
#define YANDEXNARODNETMAN_H

#include <QtNetwork>
#include <QObject>
#include <QFileDialog>

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;


class YandexNarodNetMan : public QObject
{
	Q_OBJECT

public:
	enum Action
	{
		Authorize,
		GetFileList,
		DeleteFile,
		UploadFile
	};

	YandexNarodNetMan(QObject *parent);
	~YandexNarodNetMan();

	quint32 startAuthorization();
	quint32 startAuthorization(const QString &login, const QString &password);

	void setFilepath (QString arg) { filepath = arg; }
	void startGetFilelist();
	void startDelFiles(QStringList);
	void startUploadFile(QString);

signals:
	bool authorized(quint32);

private:
	struct Track
	{
		quint32 id;
		Action action;
	};
	typedef QHash<QNetworkReply *, Track> TrackMap;
	TrackMap m_tracks;
	quint32 m_trackId;

	void netmanDo();
	QString narodCaptchaKey;
	Action m_action;
	QString page;
	QNetworkAccessManager *m_networkManager;
	QNetworkRequest m_networkRequest;
	int nstep;
	int filesnum;
	QString purl;
	QStringList fileids;
	QString filepath;
	QString lastdir;
	QFileInfo fi;
	void loadSettings();
	void loadCookies();
	void saveCookies();
	QString narodLogin;
	QString narodPasswd;
	int auth_flag;

struct FileItem
{
	QString fileicon;
	QString fileid;
	QString filename;
	QString fileurl;
};

private slots:
	void netrpFinished(QNetworkReply*);
//	void netmanTransferProgress(qint64, qint64);

signals:
	void statusText(QString);
	void statusFileName(QString);
	void progressMax(int);
	void progressValue(int);
	void newFileItem(FileItem);
	void uploadFileURL(QString);
	void transferProgress(qint64, qint64);
	void uploadFinished();
	void finished();

};

#endif // YANDEXNARODNETMAN_H

