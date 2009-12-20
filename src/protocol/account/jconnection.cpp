#include "jconnection.h"
#include <QDebug>
#include <QTimer>

namespace Jabber
{
	struct JConnectionPrivate
	{
		JAccount *account;
		Client *client;
		JConnectionBase *connection;
		QNetworkProxy proxy;
		QString resource;
		bool autoPriority;
		QMap<Presence::PresenceType, int> priority;
		QString password;
	};

	JConnection::JConnection(JAccount *account) : p(new JConnectionPrivate)
	{
		p->account = account;
		p->password = QString();
		JID jid = JID(account->jid().toStdString());
		p->client = new Client(jid, p->password.toStdString());
		p->connection = new JConnectionBase(p->client);
		loadSettings();

		p->client->setConnectionImpl(p->connection);
		p->client->disco()->setVersion("qutIM", "0.3");
		p->client->disco()->setIdentity("client", "pc");
		p->client->registerPresenceHandler(this);
	}

	void JConnection::handlePresence(const Presence &presence)
	{
		qDebug() << presence.presence();
		qDebug() << p->client->state();
		emit setStatus(presence.presence());
	}

	void JConnection::loadSettings()
	{
		QString defaultResource = JProtocol::instance()->config().group("general").value("defaultresource", QString("qutIM"));
		ConfigGroup group = p->account->config().group("general");
		/*bool lbookmark = group.value("localbookmark", false);
		if (lbookmark != p->isLocalBookmark)
		{
			p->isLocalBookmark = local_bookmark;
			requestBookmarks();
		}*/
		p->resource = group.value("resource", defaultResource);
		p->client->setResource(p->resource.toStdString());
		p->priority.clear();
		p->priority.insert(Presence::Invalid, group.value("priority", 3));
		p->autoPriority = group.value("autopriority",true);
		if (p->autoPriority)
		{
			group = JProtocol::instance()->config().group("priority");
			p->priority.insert(Presence::Available, group.value("online", 3));
			p->priority.insert(Presence::Chat, group.value("online", 3));
			p->priority.insert(Presence::Away, group.value("away", 2));
			p->priority.insert(Presence::XA, group.value("na", 1));
			p->priority.insert(Presence::DND, group.value("dnd", -1));
		}

		//proxy settings
		group = JProtocol::instance()->config().group("proxy");
		QNetworkProxy::ProxyType t = QNetworkProxy::DefaultProxy;;
		switch (group.value("type", 0)) {
			case 0: // Default proxy
				t = QNetworkProxy::DefaultProxy;
				break;
			case 1: // Socks5
				t = QNetworkProxy::Socks5Proxy;
				break;
			case 2: // No proxy
				t = QNetworkProxy::NoProxy;
				break;
			case 3: // Http
				t = QNetworkProxy::HttpProxy;
				break;
		}
		p->proxy.setType(t);
		p->proxy.setHostName(group.value("hostname", QString()));
		p->proxy.setPort(group.value("port", qint16()));
		if (group.value("authorize", false)) {
			p->proxy.setUser(group.value("user", QString()));
			p->proxy.setPassword(group.value("password", QString()));
		}
		p->connection->setProxy(p->proxy);

		//connect settings
		group = JProtocol::instance()->config().group("connect");
		TLSPolicy tls = TLSOptional;
		switch (group.value("tls", 1)) {
			case 0:
				tls = TLSDisabled;
				break;
			case 1:
				tls = TLSOptional;
				break;
			case 2:
				tls = TLSRequired;
				break;
		}
		p->client->setTls(tls);
		p->client->setSasl(group.value("sasl", true));
		p->client->setCompression(group.value("compression", true));
		if (group.value("manualhost", false))
			p->connection->setServer(group.value("server", QString::fromStdString(p->client->jid().server())).toStdString(),
					group.value("port", 5222));
		else
			p->connection->setServer(p->client->jid().server(), 5222);
		p->connection->setUseDns(group.value("usedns", true));
	}

	void JConnection::setConnectionPresence(Presence::PresenceType presence)
	{
		if (p->password.isEmpty()) {
			bool ok;
			p->password = p->account->password(&ok);
			if (ok)
				p->client->setPassword(p->password.toStdString());
			else
				return;
		}
		p->client->setPresence(presence, p->autoPriority
				? p->priority.value(presence) : p->priority.value(Presence::Invalid));
		if (p->client->state() == StateDisconnected) {
			p->client->connect(false);
		}
	}
}
