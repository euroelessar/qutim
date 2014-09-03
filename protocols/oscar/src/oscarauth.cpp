/****************************************************************************
**
** qutIM - instant messenger
**
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

#include "oscarauth.h"
#include "icqaccount.h"
#include "oscarconnection.h"
#include <qutim/json.h>
#include <qutim/libqutim_version.h>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QMessageAuthenticationCode>

#define QUTIM_DEV_ID QLatin1String("ic1wrNpw38UenMs8")
#define ICQ_LOGIN_URL "https://api.login.icq.net/auth/clientLogin"
#define ICQ_START_SESSION_URL "http://api.icq.net/aim/startOSCARSession"
#define DEBUG() if (!(*isDebug())) {} else qDebug()


namespace qutim_sdk_0_3 {

namespace oscar {

Q_GLOBAL_STATIC_WITH_ARGS(bool, isDebug, (qgetenv("QUTIM_OSCAR_DEBUG").toInt() > 0))

class OscarResponse
{
public:
	enum ResultCode
	{
		Success = 200,
		MoreAuthenticationRequired = 330,
		InvalidRequest = 400,
		AuthorizationRequired  = 401,
		MethodNoAllowed = 405,
		RequestTimeout = 408,
		SourceRateLimitReached = 430,
		InvalidKey = 440,
		KeyUsageLimitReached = 441,
		KeyInvalidIP = 442,
		MissingRequiredParameter = 460,
		SourceRequired = 461,
		ParameterError = 462,
		GenericServerError = 500,
		RateLimitReached = 607
	};
	
	OscarResponse(const QByteArray &json);
	~OscarResponse();
	
	Config data() const;
	ResultCode result() const;
	AbstractConnection::ConnectionError error() const;
	QString resultString() const;
	int detailCode() const;
	QVariantMap rawResult() const;

private:
	QVariantMap m_data;
	ResultCode m_result;
	QString m_resultString;
	QVariantMap m_rawResult;
	int m_detailCode;
};

OscarResponse::OscarResponse(const QByteArray &json)
{
	DEBUG() << json;
	QVariantMap data = Json::parse(json).toMap();
	QVariantMap response = data.value(QLatin1String("response")).toMap();
	m_rawResult = data;
	m_result = static_cast<ResultCode>(response.value(QLatin1String("statusCode")).toInt());
	m_detailCode = response.value(QLatin1String("statusDetailCode")).toInt();
	m_resultString = response.value(QLatin1String("statusText")).toString();
	m_data = response.value(QLatin1String("data")).toMap();
}

OscarResponse::~OscarResponse()
{
}

Config OscarResponse::data() const
{
	return Config(m_data);
}

OscarResponse::ResultCode OscarResponse::result() const
{
	return m_result;
}

int OscarResponse::detailCode() const
{
	return m_detailCode;
}

AbstractConnection::ConnectionError OscarResponse::error() const
{
	switch (m_result) {
	case Success:
		return AbstractConnection::NoError;
	case MoreAuthenticationRequired:
	case InvalidRequest:
		return AbstractConnection::InternalError;
	case AuthorizationRequired:
		return AbstractConnection::MismatchNickOrPassword;
	case MethodNoAllowed:
	case RequestTimeout:
		return AbstractConnection::InternalError;
	case SourceRateLimitReached:
	case RateLimitReached:
		return AbstractConnection::RateLimitExceeded;
	case InvalidKey:
	case KeyUsageLimitReached:
	case KeyInvalidIP:
	case MissingRequiredParameter:
	case SourceRequired:
	case ParameterError:
	case GenericServerError:
		return AbstractConnection::InternalError;
	default:
		return AbstractConnection::SocketError;
	}
}

QString OscarResponse::resultString() const
{
	return m_resultString;
}

QVariantMap OscarResponse::rawResult() const
{
	return m_rawResult;
}

OscarAuth::OscarAuth(IcqAccount *account) :
    QObject(account), m_account(account), m_state(Invalid)
{
	QNetworkProxy proxy = NetworkProxyManager::toNetworkProxy(NetworkProxyManager::settings(account));
	m_manager.setProxy(proxy);
	connect(account, SIGNAL(proxyUpdated(QNetworkProxy)), SLOT(setProxy(QNetworkProxy)));

	{
		// Temporary hook
		// TODO: Remove this hook or make migration more automatic
		Config cfg = m_account->config(QLatin1String("general"));
		if (cfg.hasChildGroup(QStringLiteral("token"))) {
			QVariantMap token = cfg.value(QStringLiteral("token"), QVariantMap(), Config::Crypted);
			QString data = QString::fromUtf8(Json::generate(token));
			m_keyChain->write("icq." + account->id(), data);
			cfg.remove(QStringLiteral("token"));
		}
		cfg.remove("passwd");
	}
}

OscarAuth::~OscarAuth()
{
	m_cleanupHandler.clear();
}

void OscarAuth::setProxy(const QNetworkProxy &proxy)
{
	m_manager.setProxy(proxy);
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

	if (m_passwordDialog)
		return;

	m_keyChain->read("icq." + m_account->id()).connect(this, [this] (const KeyChain::ReadResult &result) {
		QVariantMap token = Json::parse(result.textData.toUtf8()).toMap();
		if (!token.isEmpty()) {
			QByteArray a = token.value(QLatin1String("a")).toByteArray();
			QDateTime expiresAt = token.value(QLatin1String("expiresAt")).toDateTime();
			if (expiresAt > QDateTime::currentDateTime()) {
				startSession(a, token.value(QLatin1String("sessionSecret")).toByteArray());
				return;
			}
		}

		if (m_passwordDialog)
			return;

		m_passwordDialog = PasswordDialog::request(m_account);
		connect(m_passwordDialog.data(), SIGNAL(finished(int)), SLOT(onPasswordDialogFinished(int)));
	});
}

void OscarAuth::onPasswordDialogFinished(int result)
{
	PasswordDialog *dialog = qobject_cast<PasswordDialog*>(sender());
	Q_ASSERT(dialog);
	dialog->deleteLater();
	if (result == PasswordDialog::Accepted) {
		clientLogin(dialog->remember(), dialog->password());
	} else {
		m_state = AtError;
		emit stateChanged(m_state);
	}
}

static QByteArray sha256hmac(const QByteArray &array, const QByteArray &sessionSecret)
{
    QMessageAuthenticationCode code(QCryptographicHash::Sha256, sessionSecret);
    code.addData(array);
    return code.result().toBase64();
}

void OscarAuth::clientLogin(bool longTerm, const QString &password)
{
	QUrl url = QUrl::fromEncoded(ICQ_LOGIN_URL);
    QUrlQuery urlQuery;
	urlQuery.addQueryItem(QLatin1String("devId"), QUTIM_DEV_ID);
	urlQuery.addQueryItem(QLatin1String("f"), QLatin1String("json"));
	urlQuery.addQueryItem(QLatin1String("s"), m_account->id());
	urlQuery.addQueryItem(QLatin1String("language"), generateLanguage());
	urlQuery.addQueryItem(QLatin1String("tokenType"), QLatin1String(longTerm ? "longterm" : "shortterm"));
	// FIXME: Sometimes passwords are cp-1251 (or any other windows-scpecific local encoding) encoded
	urlQuery.addQueryItem("pwd", password);
	urlQuery.addQueryItem(QLatin1String("idType"), QLatin1String("ICQ"));
	urlQuery.addQueryItem(QLatin1String("clientName"), getClientName());
	urlQuery.addQueryItem(QLatin1String("clientVersion"), QString::number(version()));
	QByteArray query = urlQuery.toString(QUrl::FullyEncoded).toLatin1();
	DEBUG() << Q_FUNC_INFO << urlQuery.toString() << query;
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QNetworkReply *reply = m_manager.post(request, query);
	m_cleanupHandler.add(reply);

	connect(reply, &QNetworkReply::finished, this, [this, reply, password] () {
		onClientLoginFinished(reply, password);
	});
}

void OscarAuth::onClientLoginFinished(QNetworkReply *reply, const QString &password)
{
	reply->deleteLater();
	if (reply->error() != QNetworkReply::NoError) {
		m_errorString = reply->errorString();
		emit error(AbstractConnection::SocketError);
		deleteLater();
		return;
	}
	OscarResponse response(reply->readAll());
	DEBUG() << Q_FUNC_INFO << response.rawResult();
	if (response.result() != OscarResponse::Success) {
		m_errorString = response.resultString();
		emit error(response.error());
		deleteLater();
		return;
	}
	Config data = response.data();
	data.beginGroup(QLatin1String("token"));
	QByteArray token = data.value(QLatin1String("a"), QByteArray());
	int expiresIn = data.value(QLatin1String("expiresIn"), 0);
	QDateTime expiresAt = QDateTime::currentDateTime().addSecs(expiresIn);
	data.endGroup();
	QByteArray sessionSecret = data.value(QLatin1String("sessionSecret"), QByteArray());
	int hostTime = data.value(QLatin1String("hostTime"), 0);
	int localTime = QDateTime::currentDateTime().toUTC().toTime_t();
	sessionSecret = sha256hmac(sessionSecret, password.toUtf8());
	{
		Config cfg = m_account->config(QLatin1String("general"));
		QVariantMap data;
		data.insert(QLatin1String("a"), token);
		data.insert(QLatin1String("expiresAt"), expiresAt.toString(Qt::ISODate));
		data.insert(QLatin1String("sessionSecret"), sessionSecret);
		cfg.setValue(QLatin1String("token"), data, Config::Crypted);
		cfg.setValue(QLatin1String("hostTimeDelta"), hostTime - localTime);
	}
	startSession(token, sessionSecret);
}

void OscarAuth::startSession(const QByteArray &token, const QByteArray &sessionKey)
{
	QUrl url = QUrl::fromEncoded(ICQ_START_SESSION_URL);
    QUrlQuery query;
	query.addQueryItem("a", QString::fromLatin1(token));
	query.addQueryItem(QLatin1String("distId"), getDistId());
	query.addQueryItem(QLatin1String("f"), QLatin1String("json"));
	query.addQueryItem(QLatin1String("k"), QUTIM_DEV_ID);
	int localTime = QDateTime::currentDateTime().toUTC().toTime_t();
	{
		Config cfg = m_account->config(QLatin1String("general"));
		localTime += cfg.value(QLatin1String("hostTimeDelta"), 0);
	}
	query.addQueryItem(QLatin1String("ts"), QString::number(localTime));
//	Some strange error at ICQ servers. I receive "Parameter error" if it is set
//	url.addQueryItem(QLatin1String("language"), generateLanguage());
	query.addQueryItem(QLatin1String("majorVersion"), QString::number(versionMajor()));
	query.addQueryItem(QLatin1String("minorVersion"), QString::number(versionMinor()));
	query.addQueryItem(QLatin1String("pointVersion"), QLatin1String("0"));
	query.addQueryItem(QLatin1String("clientName"), getClientName());
	query.addQueryItem(QLatin1String("clientVersion"), QString::number(version()));
	query.addQueryItem("useTLS", m_account->connection()->isSslEnabled() ? "1" : "0");
	query.addQueryItem("sig_sha256", QString::fromLatin1(generateSignature("GET", sessionKey, url, query)));
	DEBUG() << query.toString(QUrl::FullyEncoded);
    url.setQuery(query);
	QNetworkRequest request(url);
	//	Some strange error at ICQ servers. I receive "Parameter error" if it is set no anything else
	request.setRawHeader("Accept-Language", generateLanguage().toLatin1());
	QNetworkReply *reply = m_manager.get(request);
	m_cleanupHandler.add(reply);
	connect(reply, SIGNAL(finished()), SLOT(onStartSessionFinished()));
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(onSslErrors(QList<QSslError>)));
}

void OscarAuth::onStartSessionFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	Q_ASSERT(reply);
	reply->deleteLater();
	deleteLater();
	if (reply->error() != QNetworkReply::NoError) {
		m_errorString = reply->errorString();
		emit error(AbstractConnection::SocketError);
		return;
	}
	OscarResponse response(reply->readAll());
	DEBUG() << Q_FUNC_INFO << response.rawResult();
	Config data = response.data();
	if (response.result() == OscarResponse::InvalidRequest
	        && response.detailCode() == 1015) {
		// Invalid local time
		int hostTime = data.value(QLatin1String("ts"), 0);
		int localTime = QDateTime::currentDateTime().toUTC().toTime_t();
		{
			Config cfg = m_account->config(QLatin1String("general"));
			cfg.setValue(QLatin1String("hostTimeDelta"), hostTime - localTime);
		}
		login();
		return;
	}
	if (response.result() != OscarResponse::Success) {
		m_errorString = response.resultString();
//		m_account->config(QLatin1String("general")).remove(QLatin1String("token"));
		emit error(response.error());
		return;
	}
	OscarConnection *connection = static_cast<OscarConnection*>(m_account->connection());
	QString host = data.value(QLatin1String("host"), QString());
	int port = data.value(QLatin1String("port"), 443);
	QByteArray cookie = QByteArray::fromBase64(data.value(QLatin1String("cookie"), QByteArray()));
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
	DEBUG() << Q_FUNC_INFO << str;
}

QPair<QLatin1String, QLatin1String> OscarAuth::getDistInfo() const
{
	const char *value[2] = {
#if   defined(Q_OS_WIN)
	    "21000", "QutIM Windows Client"
#elif defined(Q_OS_SYMBIAN)
	    "21030", "QutIM Symbian Client"
#elif defined(Q_WS_HAIKU)
	    "21031", "QutIM Haiku Client"
#elif defined(Q_OS_OS2)
	    "21032", "QutIM OS2 Client"
#elif defined(Q_OS_UNIX)
# if   defined(Q_OS_MAC)
#  if   defined(Q_OS_IOS)
	    "21021", "QutIM iOS Client"
#  else
	    "21020", "QutIM MacOS Client"
#  endif
# elif defined(Q_OS_LINUX)
#  if   defined(Q_WS_MAEMO_5)
		"21011", "QutIM Maemo Client"
#  elif defined(Q_WS_MEEGO) || defined(MEEGO_EDITION)
		"21012", "QutIM Meego Client"
#  elif defined(Q_WS_ANDROID)
	    "21015", "QutIM Android Client"
#  else
	    "21010", "QutIM Linux Client"
#  endif
# elif defined(Q_OS_BSD4)
	    "21013", "QutIM BSD Client"
# elif defined(Q_OS_SOLARIS)
	    "21014", "QutIM Solaris Client"
# endif
#endif // defined(Q_OS_UNIX)
	};
	// If you see error above may be we don't have DistID for your OS, please contact developers
	return qMakePair(QLatin1String(value[0]), QLatin1String(value[1]));
}

QString OscarAuth::getDistId() const
{
	return getDistInfo().first;
}

QString OscarAuth::getClientName() const
{
	return getDistInfo().second;
}

QString OscarAuth::generateLanguage()
{
	QLocale locale;
	if (locale.language() != QLocale::C)
		return locale.name().toLower().replace('_', '-');
	else
		return QLatin1String("en-us");
}

QByteArray OscarAuth::generateSignature(const QByteArray &method, const QByteArray &sessionSecret, const QUrl &url, const QUrlQuery &query)
{
    QList<QPair<QString, QString> > items = query.queryItems(QUrl::FullyDecoded);
	qSort(items);
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

    DEBUG() << "signature" << array;

	return sha256hmac(array, sessionSecret);
}

} } // namespace qutim_sdk_0_3::oscar

