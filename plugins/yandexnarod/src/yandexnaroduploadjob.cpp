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

using namespace qutim_sdk_0_3;

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
	setStateString(QT_TR_NOOP("Uploading file..."));

	m_data = setCurrentIndex(0);

	if (!m_data->open(QIODevice::ReadOnly)) {
		setError(IOError);
		setErrorString(tr("Could not open file %1").arg(fileName()));
		return;
	}

	QString fileName;
	// TODO: Think about better solution
//	fileName += QString::number(qrand(), 16);
//	fileName += "-";
	fileName += YandexNarodUploadJob::fileName();

	QUrl url(QLatin1String("https://webdav.yandex.ru/"));
	url.setPath(fileName);

	YandexRequest request(QUrl("https://webdav.yandex.ru/" + fileName));
	request.setRawHeader("Content-Length", QByteArray::number(m_data->size()));
	request.setRawHeader("Content-Type", "application/octet-stream");
	request.setRawHeader("Expect", "100-continue");

	QNetworkReply *reply = YandexNarodFactory::networkManager()->put(request, m_data);
	connect(reply, SIGNAL(finished()), this, SLOT(onUploadFinished()));
	connect(reply, SIGNAL(uploadProgress(qint64,qint64)),
			this, SLOT(onUploadProgress(qint64,qint64)));

	setState(Started);
}

bool YandexNarodUploadJob::processReply(QNetworkReply *reply)
{
	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError)
		return true;

	debug() << reply->request().url() << QString::fromUtf8(reply->readAll());
	setError(NetworkError);
	setErrorString(reply->errorString());
	return false;
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

	if (reply->error() != QNetworkReply::NoError) {
		setState(Error);
		setError(NetworkError);
		return;
	}

	int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

	if (code == 507) {
		setState(Error);
		setError(NetworkError);
		setErrorString(QT_TR_NOOP("Not anough space on storage"));
		return;
	} else if (code == 413) {
		setState(Error);
		setError(NetworkError);
		setErrorString(QT_TR_NOOP("File too large"));
		return;
	} else if (code != 201) {
		setState(Error);
		setError(NetworkError);
		return;
	}

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
}

