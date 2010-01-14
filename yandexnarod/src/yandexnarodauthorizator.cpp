#include "yandexnarodauthorizator.h"
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <QtCore/QDateTime>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QWidget>

using namespace qutim_sdk_0_3;

YandexNarodAuthorizator::YandexNarodAuthorizator(QNetworkAccessManager *parent) :
	QObject(parent), m_networkManager(parent)
{
	foreach (const QNetworkCookie &cookie,
			 parent->cookieJar()->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		if (cookie.name() == "L") {
			m_stage = Already;
			break;
		}
	}

	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onRequestFinished(QNetworkReply*)));
}

YandexNarodAuthorizator::YandexNarodAuthorizator(QWidget *parent) :
		QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{
	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onRequestFinished(QNetworkReply*)));
}

void YandexNarodAuthorizator::requestAuthorization()
{
	if (m_stage > Need) {
		if (m_stage == Already)
			emit result(Success);
		return;
	}
	ConfigGroup group = Config().group("yandexnarod");
	QString login = group.value("login", QString());
	QString password = group.value("passwd", QString(), Config::Crypted);
	return requestAuthorization(login, password);
}

void YandexNarodAuthorizator::requestAuthorization(const QString &login, const QString &password)
{
	if (m_stage > Need) {
		if (m_stage == Already)
			emit result(Success);
		return;
	}
	QByteArray post = "login=" + QUrl::toPercentEncoding(login)
					  + "&passwd=" + QUrl::toPercentEncoding(password);
	QNetworkRequest request(QUrl("https://passport.yandex.ru/passport?mode=auth"));
	request.setRawHeader("Cache-Control", "no-cache");
	request.setRawHeader("Accept", "*/*");
	QByteArray userAgent = "qutIM/";
	userAgent += qutimVersionStr();
	userAgent += " (U; YB/4.2.0; MRA/5.5; en)";
	request.setRawHeader("User-Agent", userAgent);
	m_networkManager->post(request, post);
}

void YandexNarodAuthorizator::onRequestFinished(QNetworkReply *reply)
{
	reply->deleteLater();
	if (reply->error() != QNetworkReply::NoError) {
		debug() << reply->error() << reply->errorString();
		emit result(Error, reply->errorString());
		return;
	}

	foreach (const QNetworkCookie &cookie,
			 m_networkManager->cookieJar()->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		debug() << cookie.name() << cookie.value() << cookie.expirationDate();
	}

	foreach (const QNetworkCookie &cookie,
			 m_networkManager->cookieJar()->cookiesForUrl(QUrl("http://narod.yandex.ru"))) {
		if (cookie.name() == "yandex_login" && !cookie.value().isEmpty()) {
			m_stage = Already;
			emit result(Success);
			return;
		}
	}

	QString page = QString::fromUtf8(reply->readAll());
	debug() << page;

	m_stage = Need;
	emit result(Failure);
}
