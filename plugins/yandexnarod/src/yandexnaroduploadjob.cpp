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

#include "yandexnaroduploadjob.h"
#include "yandexnarod.h"
#include <qutim/config.h>
#include <qutim/json.h>
#include <qutim/debug.h>
#include <qutim/message.h>
#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <QFileDialog>
#include <QNetworkCookieJar>
#include <QXmlQuery>
#include <QStringBuilder>

using namespace qutim_sdk_0_3;

#define WEBDAV_BASE_URL QLatin1String("https://webdav.yandex.ru/")

YandexNarodUploadJob::YandexNarodUploadJob(qutim_sdk_0_3::ChatUnit *contact,
										   YandexNarodFactory *factory) :
	FileTransferJob(contact, Outgoing, factory)
{
}

YandexNarodUploadJob::~YandexNarodUploadJob()
{
}

void YandexNarodUploadJob::doSend()
{
	YandexNarodAuthorizator *authorizator = YandexNarodFactory::authorizator();
	if (!authorizator->isAuthorized()) {
		setStateString(tr("Authorizing..."));
		connect(authorizator, SIGNAL(result(YandexNarodAuthorizator::Result,QString)),
				this, SLOT(authorizationResult(YandexNarodAuthorizator::Result,QString)));
		authorizator->requestAuthorization();
	} else {
		sendImpl();
	}
}

void YandexNarodUploadJob::doStop()
{
	// TODO: Implement
}

void YandexNarodUploadJob::doReceive()
{
	// Nothing to do
}

void YandexNarodUploadJob::sendImpl()
{
	setState(Started);
	setStateString(QT_TR_NOOP("Creating directory..."));

	m_data = setCurrentIndex(0);

	// We need only resourcetype to detect if it is a directory
	QByteArray data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>"
					  "<propfind xmlns=\"DAV:\">"
					  "<prop><resourcetype/></prop>"
					  "</propfind>";

	QUrl url(WEBDAV_BASE_URL);
	url.setPath(QLatin1String("/qutim-filetransfer/"));
	YandexRequest request(url);
	request.setRawHeader("Depth", "1");
	request.setRawHeader("Content-Length", QByteArray::number(data.size()));
	request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

	QBuffer *buffer = new QBuffer;
	buffer->setData(data);
	buffer->open(QIODevice::ReadOnly);

	QNetworkReply *reply = YandexNarodFactory::networkManager()
						   ->sendCustomRequest(request, "PROPFIND", buffer);
	buffer->setParent(reply);
	connect(reply, SIGNAL(finished()), this, SLOT(onDirectoryChecked()));
}

void YandexNarodUploadJob::uploadFile(const QUrl &url)
{
	setStateString(QT_TR_NOOP("Uploading file..."));

	if (!m_data->open(QIODevice::ReadOnly)) {
		setError(IOError);
		setErrorString(tr("Could not open file %1").arg(fileName()));
		return;
	}

	YandexRequest request(url);
	request.setRawHeader("Content-Length", QByteArray::number(m_data->size()));
	request.setRawHeader("Content-Type", "application/octet-stream");
	request.setRawHeader("Expect", "100-continue");

	QNetworkReply *putReply = YandexNarodFactory::networkManager()->put(request, m_data);
	connect(putReply, SIGNAL(finished()), this, SLOT(onUploadFinished()));
	connect(putReply, SIGNAL(uploadProgress(qint64,qint64)),
			this, SLOT(onUploadProgress(qint64,qint64)));
}

bool YandexNarodUploadJob::checkReply(QNetworkReply *reply)
{
	const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	if (code == 507) {
		setState(Error);
		setError(NetworkError);
		setErrorString(QT_TR_NOOP("Not anough space on storage"));
		return false;
	} else if (code == 413) {
		setState(Error);
		setError(NetworkError);
		setErrorString(QT_TR_NOOP("File too large"));
		return false;
	} else if (code != 201) {
		setState(Error);
		setError(NetworkError);
		return false;
	}

	return true;
}

void YandexNarodUploadJob::authorizationResult(YandexNarodAuthorizator::Result result, const QString &error)
{
	if (result == YandexNarodAuthorizator::Success) {
		sendImpl();
	} else {
		setError(NetworkError);
		setErrorString(YandexNarodFactory::authorizator()->resultString(result, error));
	}
}

void YandexNarodUploadJob::onDirectoryChecked()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	reply->deleteLater();

	const int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	if (code == 404) {
		// Directory is not created yet, so be a god
		YandexRequest request(reply->url());
		QNetworkReply *reply = YandexNarodFactory::networkManager()
							   ->sendCustomRequest(request, "MKCOL");
		connect(reply, SIGNAL(finished()), this, SLOT(onDirectoryCreated()));
		return;
	}

	const QString xmlData = QString::fromUtf8(reply->readAll());
	QStringList files;

	QXmlQuery query;
	query.setFocus(xmlData);
	query.setQuery(QLatin1String("declare default element namespace \"DAV:\";"
								 "/multistatus/response/href/text()/string()"));
	query.evaluateTo(&files);

	if (files.contains(QLatin1String("/qutim-filetransfer"))) {
		// Oops... May be check another directories?
		setState(Error);
		setError(NetworkError);
		setErrorString(QT_TR_NOOP("Can't create directory \"/qutim-filetransfer/\""));
		return;
	}

	const QString fileName = YandexNarodUploadJob::fileName();
	QUrl url = reply->url();
	QString path = url.path();
	QString filePath = path + fileName;
	if (files.contains(filePath)) {
		const QString basename = fileName.section(QLatin1Char('.'), 0, 0);
		const QString suffix = fileName.section(QLatin1Char('.'), 1);

		for (int i = 1; ; ++i) {
			filePath = path % basename
					   % QLatin1Char('(') % QString::number(i) % QLatin1Char(')')
					   % (suffix.isEmpty() ? QLatin1Literal("") : QLatin1Literal("."))
					   % suffix;
			if (!files.contains(filePath))
				break;
		}
	}

	uploadFile(reply->url().resolved(QUrl(filePath)));
}

void YandexNarodUploadJob::onDirectoryCreated()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	reply->deleteLater();

	if (!checkReply(reply))
		return;

	uploadFile(reply->url().resolved(QUrl(fileName())));
}

void YandexNarodUploadJob::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
	Q_UNUSED(bytesTotal);
	setFileProgress(bytesSent);
}

void YandexNarodUploadJob::onUploadFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	reply->deleteLater();

	if (!checkReply(reply))
		return;

	QUrl url = reply->url();
	url.setEncodedQuery("publish");
	YandexRequest request(url);
	QNetworkReply *postReply = YandexNarodFactory::networkManager()->post(request, QByteArray());
	connect(postReply, SIGNAL(finished()), SLOT(onPublishFinished()));

	setStateString(QT_TR_NOOP("Publishing file..."));
}

void YandexNarodUploadJob::onPublishFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	reply->deleteLater();

	QUrl url = reply->header(QNetworkRequest::LocationHeader).toUrl();
	if (url.isRelative())
		url = reply->url().resolved(url);

	Config config;
	config.beginGroup(QLatin1String("yandex/narod"));
	QString text = config.value("template", tr("File sent: %N (%S bytes)\n%U",
											   "Don't remove format arguments"));
	text.replace("%N", fileName());
	text.replace("%U", url.toString());
	text.replace("%S", QString::number(m_data->size()));

	Message message(text);
	message.setIncoming(false);
	message.setProperty("service", true);
	if (ChatSession *s = ChatLayer::get(chatUnit(), false))
		s->appendMessage(message);
	message.setProperty("service", false);
	chatUnit()->account()->getUnitForSession(chatUnit())->send(message);

	setState(Finished);
}

