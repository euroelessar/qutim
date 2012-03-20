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

YandexNarodBuffer::YandexNarodBuffer(const QString &fileName, QIODevice *file,
									 const QByteArray &boundary, QObject *parent) :
	QIODevice(parent)
{
	QByteArray data;
	data.append("--").append(boundary).append("\r\n");
	data.append("Content-Disposition: form-data; name=\"file\"; filename=\"")
			.append(fileName).append("\"\r\n");
	data.append("Content-Transfer-Encoding: binary\r\n");
	data.append("\r\n");

	QBuffer *buffer = new QBuffer(this);
	buffer->setData(data);
	m_devices.append(buffer);

	m_devices.append(file);
	file->setParent(this);
	connect(file, SIGNAL(destroyed()), SLOT(deleteLater()));

	data.clear();
	data.append("\r\n--").append(boundary).append("--\r\n");

	buffer = new QBuffer(this);
	buffer->setData(data);
	m_devices.append(buffer);
}

YandexNarodBuffer::~YandexNarodBuffer()
{
}

bool YandexNarodBuffer::open(OpenMode mode)
{
	bool result = true;
	foreach (QIODevice *device, m_devices)
		result &= device->open(mode);
	QIODevice::open(mode);
	return result;
}

qint64 YandexNarodBuffer::size() const
{
	quint64 totalSize = 0;
	foreach (QIODevice *device, m_devices)
		totalSize += device->size();
	return totalSize;
}

qint64 YandexNarodBuffer::readData(char *data, qint64 maxlen)
{
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

qint64 YandexNarodBuffer::writeData(const char *data, qint64 len)
{
	Q_UNUSED(data);
	Q_UNUSED(len);
	return -1;
}

YandexNarodUploadJob::YandexNarodUploadJob(qutim_sdk_0_3::ChatUnit *contact,
										   YandexNarodFactory *factory) :
	FileTransferJob(contact, Outgoing, factory)
{
	m_timer.setInterval(1000);
	m_timer.setSingleShot(true);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(someStrangeSlot()));

#if HAS_NO_TOKEN_AUTHORIZATION
	m_request.setRawHeader("Cache-Control", "no-cache");
	m_request.setRawHeader("Accept", "*/*");
	QByteArray userAgent = "qutIM/";
	userAgent += versionString();
	userAgent += " (U; YB/4.2.0; MRA/5.5; en)";
	m_request.setRawHeader("User-Agent", userAgent);
#endif
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
	// TODO: I am not sure that it's correct
	m_timer.stop();
	delete m_data.data();
}

void YandexNarodUploadJob::doReceive()
{
	// Nothing to do
}

void YandexNarodUploadJob::sendImpl()
{
	setStateString(QT_TR_NOOP("Getting storage..."));
#if HAS_NO_TOKEN_AUTHORIZATION
	m_request.setUrl(QUrl("http://narod.yandex.ru/disk/getstorage/?type=json"));

//	QNetworkCookieJar *cookieJar = YandexNarodFactory::networkManager()->cookieJar();
//	foreach(const QNetworkCookie &cookie, cookieJar->cookiesForUrl(m_request.url())) {
//		m_request.setRawHeader("Cookie", cookie.toRawForm(QNetworkCookie::NameAndValueOnly));
//		debug() << cookie;
//	}

//	debug() << "Cookie" << m_request.rawHeader("Cookie");
	QNetworkReply *reply = YandexNarodFactory::networkManager()->get(m_request);
#else
	YandexRequest request(QUrl("http://narod.yandex.ru/disk/getstorage/?type=json"));
	QNetworkReply *reply = YandexNarodFactory::networkManager()->get(request);
#endif
	connect(reply, SIGNAL(finished()), this, SLOT(storageReply()));
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

void YandexNarodUploadJob::storageReply()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	if (!processReply(reply))
		return;

	QByteArray data = reply->readAll();
	QVariantMap map = m_someData = Json::parse(data).toMap();

	debug() << "storage" << map;
	debug() << "storage" << reply->rawHeader("Set-Cookie");

	QUrl url(map.value("url").toString());
	url.addQueryItem("tid", map.value("hash").toString());

	int boundaryTemp[] = { qrand(), qrand(), qrand() };
	QByteArray boundary = QByteArray::fromRawData(reinterpret_cast<char *>(boundaryTemp),
												  sizeof(boundaryTemp)).toHex();
	m_data = setCurrentIndex(0);
	m_data = new YandexNarodBuffer(fileName(), m_data.data(), boundary, this);
	if (!m_data.data()->open(QIODevice::ReadOnly)) {
		setError(IOError);
		setErrorString(tr("Could not open file %1").arg(fileName()));
		return;
	}

	setState(Started);
	
#if HAS_NO_TOKEN_AUTHORIZATION
	QNetworkRequest request(m_request);
	request.setUrl(url);
#else
	YandexRequest request(url);
#endif
	request.setRawHeader("Content-Type", "multipart/form-data, boundary=" + boundary);
	request.setRawHeader("Content-Length", QString::number(m_data.data()->size()).toLatin1());

	QNetworkReply *uploadNetworkReply = YandexNarodFactory::networkManager()->post(request, m_data.data());
	connect(m_data.data(), SIGNAL(destroyed()), uploadNetworkReply, SLOT(deleteLater()));
	connect(uploadNetworkReply, SIGNAL(finished()), this, SLOT(uploadReply()));

	m_timer.start();
}

void YandexNarodUploadJob::someStrangeSlot()
{
	QUrl url(m_someData.value("purl").toString());
	url.addQueryItem("tid", m_someData.value("hash").toString());
//	url.addQueryItem("type", "json");
#if HAS_NO_TOKEN_AUTHORIZATION
	m_request.setUrl(url);
	QNetworkReply *reply = YandexNarodFactory::networkManager()->get(m_request);
#else
	YandexRequest request(url);
	QNetworkReply *reply = YandexNarodFactory::networkManager()->get(request);
#endif
	connect(m_data.data(), SIGNAL(destroyed()), reply, SLOT(deleteLater()));
	connect(reply, SIGNAL(finished()), SLOT(progressReply()));
}

void YandexNarodUploadJob::uploadReply()
{
	m_timer.stop();
	debug() << "uploadReply";
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	if (!processReply(reply))
		return;

	QByteArray data = reply->readAll();
	QVariantMap map = Json::parse(data).toMap();

	debug() << "upload" << data << map;
	debug() << reply->rawHeaderList();
	debug() << "upload" << reply->rawHeader("Set-Cookie");

	someStrangeSlot();
}

void YandexNarodUploadJob::progressReply()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	if (!processReply(reply))
		return;

	QByteArray data = reply->readAll();
#if HAS_NO_TOKEN_AUTHORIZATION
	int start = data.indexOf('{');
	int end = data.lastIndexOf('}');
	QVariantMap map = Json::parse(data.mid(start, end - start + 1)).toMap();
#else
	QVariant tmpVar;
	int dataBegin = data.indexOf('(') + 1;
	int dataEnd = data.lastIndexOf(')');
	int dataLength = dataEnd - dataBegin;
	if (dataBegin < 0 || dataEnd < 0 || dataLength < 0) {
		dataBegin = 0;
		dataLength = data.length();
	}
	Json::parseRecord(tmpVar, data.constData() + dataBegin, &dataLength);
	QVariantMap map = tmpVar.toMap();
#endif

	debug() << "progress" << reply->request().url() << data << map;
	debug() << "progress" << reply->rawHeader("Set-Cookie");
	int newProgress = fileSize() * (map.value("percent").toDouble() / 100.0);
	if (newProgress > progress())
		setFileProgress(newProgress);

	QString status = map.value("status").toString();
	if (status == "upload") {
		m_timer.start();
//		{
//		"tid": "1263553646SvsF9FFimF5thmoM2kGTwxL9",
//		"status": "upload",
//		"percent": "94.11",
//		"ipercent": 94,
//		"random": "1263553668S4gHxbB2nJSd2clhhfYimsfM",
//		"time": { "min": "0","sec": "01" }
//		}
	} else if (status == "done" && state() != Finished) {
//		{
//		"tid": "1263553646SvsF9FFimF5thmoM2kGTwxL9",
//		"status": "done",
//		"fids": "8433938",
//		"files": [{ "fid": "8433938", "hash": "16928325000", "name": "Makefile", "size": "146305" }],
//		"percent": "100.00",
//		"ipercent": 100,
//		"random": "1263553670cshMPkykOsHbaCnIQ5ehEOKd",
//		"time": { "min": "0","sec": "00" }
//		}

		setState(Finished);
		QVariantMap varMap = map.value("files").toList().value(0).toMap();
		if (!varMap.isEmpty() && chatUnit()) {
			debug() << "done" << chatUnit()->title() << varMap;

			QString url(QLatin1String("http://narod.ru/disk/"));
			url += varMap.value("hash").toString();
			url += QLatin1Char('/');
			url += varMap.value("name").toString();
			url += QLatin1String(".html");
			debug() << url;

			QString sendmsg = Config().group("yandex").group("narod")
							  .value("template", tr("File sent: %N (%S bytes)\n%U",
													"Don't remove format arguments"));
			sendmsg.replace("%N", varMap.value("name").toString());
			sendmsg.replace("%U", url);
			sendmsg.replace("%S", varMap.value("size").toString());

			//Append message to chatunit
			Message msg(sendmsg);
			msg.setIncoming(false);
			msg.setProperty("service", true);
			if (ChatSession *s = ChatLayer::get(chatUnit(), false))
				s->appendMessage(msg);
			msg.setProperty("service", false);
			chatUnit()->account()->getUnitForSession(chatUnit())->send(msg);
		}
	}
}

