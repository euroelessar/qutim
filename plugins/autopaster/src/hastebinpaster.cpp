#include "hastebinpaster.h"
#include <qutim/json.h>

using namespace qutim_sdk_0_3;

HastebinPaster::HastebinPaster()
{
}

QString HastebinPaster::name()
{
	return QLatin1String("hastebin.com");
}

QNetworkReply *HastebinPaster::send(QNetworkAccessManager *manager, const QString &content, const QString &syntax)
{
	Q_UNUSED(syntax);
	QNetworkRequest request(QUrl(QLatin1String("http://hastebin.com/documents")));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	return manager->post(request, content.toUtf8());
}

QUrl HastebinPaster::handle(QNetworkReply *reply, QString *)
{
	QString id = Json::parse(reply->readAll()).toMap().value("key").toString();
	QUrl url("http://hastebin.com/");
	url.setPath(id);
	return url;
}
