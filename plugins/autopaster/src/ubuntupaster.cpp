#include "ubuntupaster.h"
#include <QHttpMultiPart>

UbuntuPaster::UbuntuPaster()
{
}

QString UbuntuPaster::name()
{
	return QLatin1String("paste.ubuntu.com");
}

QNetworkReply *UbuntuPaster::send(QNetworkAccessManager *manager, const QString &content, const QString &syntax)
{
	QHttpMultiPart *multi = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	appendPart(multi, "poster", "qutim");
	appendPart(multi, "syntax", syntax.toUtf8());
	appendPart(multi, "content", content.toUtf8());

	QNetworkRequest request(QUrl(QLatin1String("http://paste.ubuntu.com")));
	QNetworkReply *reply = manager->post(request, multi);
	multi->setParent(reply);
	return reply;
}

QUrl UbuntuPaster::handle(QNetworkReply *reply, QString *)
{
	QUrl url = reply->header(QNetworkRequest::LocationHeader).toUrl();
	return reply->url().resolved(url);
}

void UbuntuPaster::appendPart(QHttpMultiPart *multi, const QByteArray &name, const QByteArray &value)
{
	QHttpPart part;
	part.setHeader(QNetworkRequest::ContentDispositionHeader,
				   QVariant("form-data; name=\"" + name + "\""));
	part.setBody(value);
	multi->append(part);
}
