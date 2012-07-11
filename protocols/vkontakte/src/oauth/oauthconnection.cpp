#include "oauthconnection.h"
#include <QWebPage>
#include <QWebFrame>
#include <QPointer>
#include <QDateTime>
#include <QStringBuilder>
#include <QNetworkRequest>

#include <qutim/json.h>

#include <QDebug>
#include <QWebElement>

namespace vk {

const static QUrl authUrl("http://api.vk.com/oauth/authorize");
const static QUrl apiUrl("https://api.vk.com/method/");

class OAuthConnection;
class OAuthConnectionPrivate
{
	Q_DECLARE_PUBLIC(OAuthConnection)
public:
	OAuthConnectionPrivate(OAuthConnection *q, int clientId) : q_ptr(q),
		clientId(clientId),
		scope(QStringList() << QLatin1String("notify")
							<< QLatin1String("friends")
							<< QLatin1String("photos")
							<< QLatin1String("audio")
							<< QLatin1String("video")
							<< QLatin1String("docs")
							<< QLatin1String("notes")
							<< QLatin1String("pages")
							<< QLatin1String("status")
							<< QLatin1String("offers")
							<< QLatin1String("questions")
							<< QLatin1String("wall")
							<< QLatin1String("groups")
							<< QLatin1String("messages")
							<< QLatin1String("notifications")
							<< QLatin1String("stats")
							<< QLatin1String("ads")),
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
	int clientId; //appId
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
	void setConnectionState(Client::State state);
	void _q_loadFinished(bool);
};


OAuthConnection::OAuthConnection(int appId, QObject *parent) :
	Connection(parent),
	d_ptr(new OAuthConnectionPrivate(this, appId))
{
}

OAuthConnection::~OAuthConnection()
{
}

void OAuthConnection::connectToHost(const QString &login, const QString &password)
{
	Q_D(OAuthConnection);
	if (d->login != login || d->password != password) {
		d->accessToken.clear();
		d->expiresIn = QDateTime();
		d->login = login;
		d->password = password;
	}
	if (d->accessToken.isNull() || d->expiresIn < QDateTime::currentDateTime()) {
		d->requestToken();
		d->setConnectionState(Client::StateConnecting);
	} else
		d->setConnectionState(Client::StateOnline);
}

void OAuthConnection::disconnectFromHost()
{
	Q_D(OAuthConnection);
	d->setConnectionState(Client::StateOffline);
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
	url.addQueryItem(QLatin1String("client_id"), QByteArray::number(clientId));
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
	QVariantMap response = qutim_sdk_0_3::Json::parse(webPage->mainFrame()->toPlainText().toUtf8()).toMap();
	if (ok && response.value("error").isNull()) {
		url = QUrl("http://foo.bar?" + url.fragment()); //evil hack for parse fragment as query items
		if (!url.hasEncodedQueryItem("access_token")) {
			if (!webPage->view()) {
				QWebFrame *frame = webPage->mainFrame();

				QWebElement element = frame->findFirstElement("input[name=email]");
				element.setAttribute("value", login);
				element = frame->findFirstElement("input[name=pass]");
				element.setAttribute("value", password);
				element = frame->findFirstElement("#login_enter");
				element.setFocus();
				emit q->authConfirmRequested(webPage.data());
			}
		} else {
			accessToken = url.encodedQueryItemValue("access_token");
			expiresIn = QDateTime::fromTime_t(QDateTime::currentDateTime().toTime_t() +
											  url.encodedQueryItemValue("expires_in").toUInt());
			uid = url.encodedQueryItemValue("user_id").toInt();
			setConnectionState(Client::StateOnline);
			webPage->deleteLater();
		}
	} else {
		setConnectionState(Client::StateOffline);
		emit q->error(Client::AuthorizationError);
	}
}

} // namespace vk

#include "oauthconnection.moc"
