/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "oscarauth.h"
#include "icqaccount.h"
#include "oscarconnection.h"
#include <qutim/json.h>
#include <qutim/libqutim_version.h>
#include <qutim/passworddialog.h>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QtCrypto>

#define QUTIM_DEV_ID QLatin1String("ic1wrNpw38UenMs8")
#define ICQ_LOGIN_URL "https://api.login.icq.net/auth/clientLogin"
#define ICQ_START_SESSION_URL "https://api.icq.net/aim/startOSCARSession"


namespace qutim_sdk_0_3 {

namespace oscar {

OscarAuth::OscarAuth(IcqAccount *account) :
    QObject(account), m_account(account), m_state(Invalid)
{
	QCA::init();
}

void OscarAuth::login()
{
	Config cfg = m_account->config(QLatin1String("general"));
	QVariantMap token = cfg.value(QLatin1String("token"), QVariantMap(), Config::Crypted);
	if (!token.isEmpty()) {
		QByteArray a = token.value(QLatin1String("a")).toByteArray();
		QDateTime expiresAt = token.value(QLatin1String("expiresAt")).toDateTime();
		if (expiresAt > QDateTime::currentDateTime()) {
			startSession(a, token.value(QLatin1String("sessionSecret")).toByteArray());
			return;
		}
	}
	QString password;
	{
		password = cfg.value(QLatin1String("passwd"), QString(), Config::Crypted);
		if (password.isEmpty()) {
			PasswordDialog *dialog = PasswordDialog::request(m_account);
			connect(dialog, SIGNAL(finished(int)), SLOT(onPasswordDialogFinished(int)));
			return;
		}
		m_password = password;
	}
	clientLogin(true);
}

void OscarAuth::onPasswordDialogFinished(int result)
{
	PasswordDialog *dialog = qobject_cast<PasswordDialog*>(sender());
	Q_ASSERT(dialog);
	dialog->deleteLater();
	if (result == PasswordDialog::Accepted) {
		m_password = dialog->password();
		clientLogin(dialog->remember());
	} else {
		m_state = AtError;
		emit stateChanged(m_state);
	}
}

void OscarAuth::onClienLoginFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	Q_ASSERT(reply);
	QVariantMap replyData = Json::parse(reply->readAll()).toMap();
	qDebug() << Q_FUNC_INFO << reply->error() << reply->errorString() << replyData;
	QVariantMap response = replyData.value(QLatin1String("response")).toMap().value(QLatin1String("data")).toMap();
	QVariantMap tokenData = response.value(QLatin1String("token")).toMap();
	QByteArray token = tokenData.value(QLatin1String("a")).toByteArray();
	int expiresIn = tokenData.value(QLatin1String("expiresIn")).toInt();
	QDateTime expiresAt = QDateTime::currentDateTime().addSecs(expiresIn);
	QByteArray sessionSecret = response.value(QLatin1String("sessionSecret")).toByteArray();
	{
		QCA::MessageAuthenticationCode hash(QLatin1String("hmac(sha256)"), m_password.toUtf8());
		hash.update(sessionSecret);
		sessionSecret = hash.final().toByteArray().toBase64();
	}
	{
		Config cfg = m_account->config(QLatin1String("general"));
		QVariantMap data;
		data.insert(QLatin1String("a"), token);
		data.insert(QLatin1String("expiresAt"), expiresAt.toString(Qt::ISODate));
		data.insert(QLatin1String("sessionSecret"), sessionSecret);
		cfg.setValue(QLatin1String("token"), data, Config::Crypted);
	}
	startSession(token, sessionSecret);
}

void OscarAuth::clientLogin(bool longTerm)
{
	QUrl url = QUrl::fromEncoded(ICQ_LOGIN_URL);
	url.addQueryItem(QLatin1String("devId"), QUTIM_DEV_ID);
	url.addQueryItem(QLatin1String("f"), QLatin1String("json"));
	url.addQueryItem(QLatin1String("s"), m_account->id());
	url.addQueryItem(QLatin1String("language"), generateLanguage());
	url.addQueryItem(QLatin1String("tokenType"), QLatin1String(longTerm ? "longterm" : "shortterm"));
	url.addQueryItem(QLatin1String("pwd"), m_password.toUtf8());
	url.addQueryItem(QLatin1String("idType"), QLatin1String("ICQ"));
	url.addQueryItem(QLatin1String("clientName"), getClientName());
	url.addQueryItem(QLatin1String("clientVersion"), QString::number(version()));
	QByteArray query = url.encodedQuery();
	url.setEncodedQuery(QByteArray());
	QNetworkRequest request(url);
	QNetworkReply *reply = m_manager.post(request, query);
	connect(reply, SIGNAL(finished()), this, SLOT(onClienLoginFinished()));
}

void OscarAuth::startSession(const QByteArray &token, const QByteArray &sessionKey)
{
	QUrl url = QUrl::fromEncoded(ICQ_START_SESSION_URL);
	url.addQueryItem(QLatin1String("k"), QUTIM_DEV_ID);
	url.addQueryItem(QLatin1String("f"), QLatin1String("json"));
	url.addEncodedQueryItem("a", token);
	url.addQueryItem(QLatin1String("language"), generateLanguage());
	url.addQueryItem(QLatin1String("distId"), getDistId());
	url.addQueryItem(QLatin1String("majorVersion"), QString::number(versionMajor()));
	url.addQueryItem(QLatin1String("minorVersion"), QString::number(versionMinor()));
	url.addQueryItem(QLatin1String("pointVersion"), QLatin1String("0"));
	url.addQueryItem(QLatin1String("clientName"), getClientName());
	url.addQueryItem(QLatin1String("clientVersion"), QString::number(version()));
	url.addEncodedQueryItem("useTLS", m_account->connection()->isSslEnabled() ? "true" : "false");
	url.addQueryItem(QLatin1String("ts"), QString::number(QDateTime::currentDateTime().toUTC().toTime_t()));
	url.addEncodedQueryItem("sig_sha256", generateSignature("POST", sessionKey, url));
	QByteArray data = url.encodedQuery();
	url.setEncodedQuery(QByteArray());
	QNetworkRequest request(url);
	QNetworkReply *reply = m_manager.post(request, data);
	connect(reply, SIGNAL(finished()), SLOT(onStartSessionFinished()));
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(onSslErrors(QList<QSslError>)));
}

void OscarAuth::onStartSessionFinished()
{
	deleteLater();
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT(reply);
	reply->deleteLater();
	QVariantMap replyData = Json::parse(reply->readAll()).toMap();
	qDebug() << Q_FUNC_INFO << reply->error() << reply->errorString() << replyData;
	replyData = replyData.value(QLatin1String("response")).toMap();
	replyData = replyData.value(QLatin1String("data")).toMap();
	OscarConnection *connection = static_cast<OscarConnection*>(m_account->connection());
	QString host = replyData.value(QLatin1String("host")).toString();
	int port = replyData.value(QLatin1String("port")).toInt();
	QByteArray cookie = QByteArray::fromBase64(replyData.value(QLatin1String("cookie")).toByteArray());
	qDebug() << host << port;
	connection->connectToBOSS(host, port, cookie);
}

void OscarAuth::onSslErrors(const QList<QSslError> &errors)
{
	QString str;
	foreach (const QSslError &error, errors) {
		str += "(" + QString::number(error.error()) + ") ";
		str += error.errorString();
		str += '\n';
	}
	str.chop(1);
	qDebug() << Q_FUNC_INFO << str;
}

QString OscarAuth::getDistId()
{
	return QLatin1String("21000");
}

QString OscarAuth::getClientName()
{
	return QLatin1String("QutIM Windows Client");
}

QString OscarAuth::generateLanguage()
{
	QLocale locale;
	if (locale.language() != QLocale::C)
		return locale.name().toLower().replace('_', '-');
	else
		return QLatin1String("en-us");
}

QByteArray OscarAuth::generateSignature(const QByteArray &method, const QByteArray &sessionSecret, const QUrl &url)
{
	QList<QPair<QString, QString> > items = url.queryItems();
	qSort(items);
	QCA::MessageAuthenticationCode hash(QLatin1String("hmac(sha256)"), sessionSecret);
	QByteArray array = method;
	array += '&';
	QString str;
	str = url.toString(QUrl::RemoveUserInfo | QUrl::RemovePort
	                   | QUrl::RemoveQuery | QUrl::RemoveFragment);
	array += QUrl::toPercentEncoding(str);
	array += '&';
	str.clear();
	for (int i = 0; i < items.size(); ++i) {
		str += items[i].first;
		str += QLatin1String("=");
		str += QLatin1String(QUrl::toPercentEncoding(items[i].second));
		str += QLatin1String("&");
	}
	str.chop(1);
	array += QUrl::toPercentEncoding(str, QByteArray(), "&=");
	hash.update(array);
	return hash.final().toByteArray().toBase64();
}

} } // namespace qutim_sdk_0_3::oscar
