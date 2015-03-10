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
	QNetworkRequest request(QUrl(QStringLiteral("http://paste.kde.org/api/json/create")));
	request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

    QUrlQuery data;
	data.addQueryItem(QStringLiteral("data"), content);
	data.addQueryItem(QStringLiteral("language"), syntax);
	data.addQueryItem(QStringLiteral("private"), QStringLiteral("true"));

	return manager->post(request, data.query(QUrl::FullyEncoded).toLatin1());
}

QUrl KdePaster::handle(QNetworkReply *reply, QString *error)
{
	QVariantMap result = Json::parse(reply->readAll()).toMap().value(QStringLiteral("result")).toMap();
	*error = result.value(QStringLiteral("error")).toString();

	QString id = result.value(QStringLiteral("id")).toString();
	QString hash = result.value(QStringLiteral("hash")).toString();

	QString url = QStringLiteral("http://paste.kde.org/") + id;
	if (!hash.isEmpty()) {
		url += QLatin1Char('/');
		url += hash;
	}

	return url;
}
