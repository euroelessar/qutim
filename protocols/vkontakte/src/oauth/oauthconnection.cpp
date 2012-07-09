#include "oauthconnection.h"
#include <QWebPage>
#include <QWebFrame>
#include <QPointer>
#include <QDateTime>
#include <QStringBuilder>
#include <QNetworkRequest>

namespace vk {

const static QUrl authUrl("http://api.vk.com/oauth/authorize");
const static QUrl apiUrl("https://api.vk.com/method/");

class OAuthConnection;
class OAuthConnectionPrivate
{
	Q_DECLARE_PUBLIC(OAuthConnection)
public:
	OAuthConnectionPrivate(OAuthConnection *q) : q_ptr(q),
		clientId(QLatin1String("qutIM")),
		scope(QStringList() << QLatin1String("notify")
							<< QLatin1String("friends")
							<< QLatin1String("photos")
							<< QLatin1String("audio")
							<< QLatin1String("video")
							<< QLatin1String("docs")
							<< QLatin1String("notes")),
		redirectUri(QLatin1String("http://oauth.vk.com/blank.html")),
		displayType(OAuthConnection::Touch),
		responseType(QLatin1String("token")),
		uid(0)
	{

	}
	OAuthConnection *q_ptr;
	QPointer<QWebPage> webPage;
	vk::Client::State connectionState;

	//OAuth settings
	QString clientId; //appId
	QStringList scope; //settings
	QString redirectUri;
	OAuthConnection::DisplayType displayType;
	QString responseType;

	QString login;
	QString password;

	//response
	QByteArray accessToken;
	int uid;
	QDateTime expiresIn;

	void requestToken();
	void setConnectionState(vk::Client::State state);
	void _q_loadFinished(bool);
};


OAuthConnection::OAuthConnection(QObject *parent) :
	Connection(parent),
	d_ptr(new OAuthConnectionPrivate(this))
{
}

void OAuthConnection::connectToHost(const QString &login, const QString &password)
{
}

void OAuthConnection::disconnectFromHost()
{
}

QNetworkReply *OAuthConnection::request(const QString &method, const QVariantMap &args)
{
	Q_D(OAuthConnection);

	//TODO test expiresIn
	QUrl url = apiUrl;
	url.setPath(url.path() % QLatin1Literal("/") % method);
	QVariantMap::const_iterator it = args.constBegin();
	for (; it != args.constEnd(); it++)
		url.addQueryItem(it.key(), it.value().toString());
	url.addEncodedQueryItem("access_token", d->accessToken);

	QNetworkRequest request(url);
	QNetworkReply *reply = get(request);
	return reply;
}

Client::State OAuthConnection::connectionState() const
{
	return d_func()->connectionState;
}

int OAuthConnection::uid() const
{
	return d_func()->uid;
}

void OAuthConnectionPrivate::requestToken()
{
	Q_Q(OAuthConnection);
	if (!webPage) {
		webPage = new QWebPage(q);
		webPage->setNetworkAccessManager(q);
		q->connect(webPage->mainFrame(), SIGNAL(loadFinished(bool)), SLOT(_q_loadFinished(bool)));
	}
	QUrl url = authUrl;
	url.addQueryItem(QLatin1String("client_id"), clientId);
	url.addQueryItem(QLatin1String("scope"), scope.join(","));
	url.addQueryItem(QLatin1String("redirect_uri"), redirectUri);
	const char *type[] = {
		"page",
		"popup",
		"touch",
		"wap"
	};
	url.addQueryItem(QLatin1String("display"), type[displayType]);
	url.addQueryItem(QLatin1String("response_type"), responseType);
	webPage->mainFrame()->load(url);
}

void OAuthConnectionPrivate::setConnectionState(Client::State state)
{
	Q_Q(OAuthConnection);
	if (state != connectionState) {
		connectionState = state;
		emit q->connectionStateChanged(state);
	}
}

void OAuthConnectionPrivate::_q_loadFinished(bool ok)
{
	Q_Q(OAuthConnection);
	QUrl url = webPage->mainFrame()->url();
	if (!ok || !url.encodedQueryItemValue("error").isEmpty()) {
		if (webPage->mainFrame()->url() == authUrl) {
			emit q->authConfirmRequested(webPage.data());
		} else {
			accessToken = url.encodedQueryItemValue("access_token");
			expiresIn = QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() +
											  url.encodedQueryItemValue("expires_in").toUInt());
			uid = url.encodedQueryItemValue("user_id").toInt();
		}
	} else {
		setConnectionState(vk::Client::StateOffline);
		emit q->error(vk::Client::AuthorizationError);
	}
}

} // namespace vk

#include "oauthconnection.moc"
