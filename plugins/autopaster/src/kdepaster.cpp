#include "kdepaster.h"
#include <qutim/json.h>
#include <QUrlQuery>

using namespace qutim_sdk_0_3;

KdePaster::KdePaster()
{
}

QString KdePaster::name()
{
	return QLatin1String("paste.kde.org");
}

QNetworkReply *KdePaster::send(QNetworkAccessManager *manager, const QString &content, const QString &syntax)
{
	QNetworkRequest request(QUrl(QLatin1String("http://paste.kde.org/")));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery data;
    data.addQueryItem("paste_data", content);
	data.addQueryItem("paste_lang", syntax);
	data.addQueryItem("api_submit", "true");
	data.addQueryItem("mode", "json");
    return manager->post(request, data.query(QUrl::FullyEncoded).toUtf8());
}

QUrl KdePaster::handle(QNetworkReply *reply, QString *error)
{
	QVariantMap result = Json::parse(reply->readAll()).toMap().value("result").toMap();
	*error = result.value("error").toString();
	return QUrl(QLatin1String("http://paste.kde.org/") + result.value("id").toString());
}
