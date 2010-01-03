#include "jconnection.h"
#include "../../jprotocol.h"
#include "../jaccount.h"
#include "sdk/jabber.h"
#include <gloox/adhoc.h>
#include <gloox/vcardmanager.h>
#include <qutim/debug.h>
#include <qutim/systeminfo.h>
#include <gloox/capabilities.h>

namespace Jabber
{
	struct JConnectionPrivate
	{
		JConnectionPrivate() {}
		~JConnectionPrivate() {}
		JAccount *account;
		Client *client;
		Adhoc *adhoc;
		VCardManager *vCardManager;
		JConnectionTCPBase *connection;
		QNetworkProxy proxy;
		QString resource;
		bool autoPriority;
		QMap<Presence::PresenceType, int> priority;
		QString password;
		QList<JabberExtension *> extensions;
	};

	JConnection::JConnection(JAccount *account) : p(new JConnectionPrivate)
	{
		p->account = account;
		p->password = QString();
		JID jid = JID(account->jid().toStdString());
		p->client = new Client(jid, p->password.toStdString());
		p->adhoc = new Adhoc(p->client);
		p->vCardManager = new VCardManager(p->client);
		p->connection = new JConnectionTCPBase(p->client);
		loadSettings();

		Capabilities *caps = new Capabilities(p->client->disco());
		caps->setNode("http://qutim.org");
		p->client->addPresenceExtension(caps);

		p->client->setConnectionImpl(p->connection);
		p->client->disco()->setVersion("qutIM", qutimVersionStr());
		p->client->disco()->setIdentity("client", "pc");
		p->client->disco()->addFeature("jabber:iq:roster");
		p->client->registerPresenceHandler(this);

		// XEP-0232
		DataForm *form = new DataForm(TypeResult);
		form->addField(DataFormField::TypeHidden, "FORM_TYPE", "urn:xmpp:dataforms:softwareinfo");
		form->addField(DataFormField::TypeNone, "os", SystemInfo::getName().toStdString());
		form->addField(DataFormField::TypeNone, "os_version", SystemInfo::getVersion().toStdString());
		form->addField(DataFormField::TypeNone, "software", "qutIM");
		form->addField(DataFormField::TypeNone, "software_version", qutimVersionStr());
		p->client->disco()->setForm(form);

		JabberParams params;
		params.addItem<Client>(p->client);
		params.addItem<Adhoc>(p->adhoc);
		params.addItem<VCardManager>(p->vCardManager);

		foreach (const ObjectGenerator *gen, moduleGenerators<JabberExtension>()) {
			if (JabberExtension *ext = gen->generate<JabberExtension>()) {
				p->extensions.append(ext);
				ext->init(p->account, params);
			}
		}
	}

	JConnection::~JConnection()
	{
		qDeleteAll(p->extensions);
	}

	Client *JConnection::client()
	{
		return p->client;
	}

	void JConnection::handlePresence(const Presence &presence)
	{
		p->account->endChangeStatus(presence.presence());
	}

	void JConnection::loadSettings()
	{
		// TODO: FIXME: Protocol settings must be fallback for account everywhere %)
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
		group = p->account->config().group("proxy");
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
		group = p->account->config().group("connect");
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
			p->client->setXmlLang(QLocale().name().section('_', 0, 0).toStdString());
			p->client->connect(false);
		}
		if (presence == Presence::Unavailable)
			p->client->disconnect();
	}
}
