#include "jconnection.h"
#include "../../jprotocol.h"
#include "../jaccount.h"
#include "../roster/jsoftwaredetection.h"
#include "sdk/jabber.h"
#include "../vcard/jvcardmanager.h"
#include <gloox/adhoc.h>
#include <qutim/systeminfo.h>
#include <gloox/capabilities.h>
#include <gloox/receipt.h>
#include <gloox/vcardupdate.h>

namespace Jabber
{
	struct JConnectionPrivate
	{
		JConnectionPrivate() {}
		~JConnectionPrivate() {}
		JAccount *account;
		Client *client;
		Adhoc *adhoc;
		JVCardManager *vCardManager;
		JConnectionTCPBase *connection;
		QNetworkProxy proxy;
		QString resource;
		bool autoPriority;
		QMap<Presence::PresenceType, int> priority;
		QString password;
		QList<JabberExtension *> extensions;
		QString avatarHash;
	};

	JConnection::JConnection(JAccount *account) : p(new JConnectionPrivate)
	{
		p->account = account;
		p->password = QString();
		JID jid = JID(account->id().toStdString());
		p->client = new Client(jid, p->password.toStdString());
		p->adhoc = new Adhoc(p->client);
		p->vCardManager = new JVCardManager(p->account, p->client);
		p->connection = new JConnectionTCPBase(p->client);
		loadSettings();

		p->client->registerStanzaExtension(new Receipt(Receipt::Invalid));
		p->client->registerStanzaExtension(new VCardUpdate());
		p->client->addPresenceExtension(new VCardUpdate(p->avatarHash.toStdString()));
		Capabilities *caps = new Capabilities(p->client->disco());
		caps->setNode("http://qutim.org");
		p->client->addPresenceExtension(caps);

		p->client->setConnectionImpl(p->connection);
		p->client->disco()->setVersion("qutIM", qutimVersionStr(),
									   SystemInfo::getFullName().toStdString());
		p->client->disco()->setIdentity("client", "pc", "qutIM");
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

		p->client->disco()->addFeature("http://jabber.org/protocol/chatstates");
//		p->client->disco()->addFeature("http://jabber.org/protocol/bytestreams");
//		p->client->disco()->addFeature("http://jabber.org/protocol/si");
//		p->client->disco()->addFeature("http://jabber.org/protocol/si/profile/file-transfer");
		p->client->disco()->addFeature("http://jabber.org/protocol/disco#info");
//		p->client->disco()->addFeature("http://jabber.org/protocol/commands");
//		p->client->disco()->addFeature("http://jabber.org/protocol/rosterx");
//		p->client->disco()->addFeature("http://jabber.org/protocol/muc");
		p->client->disco()->addFeature("jabber:x:data");
//		p->client->disco()->addFeature("http://jabber.org/protocol/activity+notify");
//		p->client->disco()->addFeature("http://jabber.org/protocol/mood+notify");
//		p->client->disco()->addFeature("http://jabber.org/protocol/tune+notify");
//		p->client->disco()->addFeature("http://jabber.org/protocol/physloc+notify");
//		p->client->disco()->addFeature("http://jabber.org/protocol/geoloc+notify");
//		p->client->disco()->addFeature("http://www.xmpp.org/extensions/xep-0084.html#ns-metadata+notify");
		p->client->disco()->addFeature("urn:xmpp:receipts");
		p->client->disco()->addFeature("http://jabber.org/protocol/xhtml-im");
//		p->client->disco()->addFeature("urn:xmpp:tmp:sxe");
//		p->client->disco()->addFeature("http://www.w3.org/2000/svg");
	}

	JConnection::~JConnection()
	{
		qDeleteAll(p->extensions);
	}

	Client *JConnection::client()
	{
		return p->client;
	}

	Adhoc *JConnection::adhoc()
	{
		return p->adhoc;
	}

	JVCardManager *JConnection::vCardManager()
	{
		return p->vCardManager;
	}

	void JConnection::initExtensions()
	{
		JabberParams params;
		params.addItem<Client>(p->client);
		params.addItem<Adhoc>(p->adhoc);
		params.addItem<VCardManager>(p->vCardManager->manager());

		new JSoftwareDetection(p->account, params);

		foreach (const ObjectGenerator *gen, moduleGenerators<JabberExtension>()) {
			if (JabberExtension *ext = gen->generate<JabberExtension>()) {
				p->extensions.append(ext);
				ext->init(p->account, params);
			}
		}
	}

	void JConnection::handlePresence(const Presence &presence)
	{
		if (presence.from() == p->client->jid())
			p->account->endChangeStatus(presence.presence());
	}

	void JConnection::loadSettings()
	{
		// TODO: FIXME: Protocol settings must be fallback for account everywhere %)
		QString defaultResource = JProtocol::instance()->config().group("general").value("defaultresource", QString("qutIM"));
		ConfigGroup group = p->account->config().group("general");
		p->resource = group.value("resource", defaultResource);
		p->avatarHash = group.value("avatarhash", QString());
		p->client->setResource(p->resource.toStdString());
		p->priority.clear();
		p->priority.insert(Presence::Invalid, group.value("priority", 3));
		p->autoPriority = group.value("autopriority",true);
		if (p->autoPriority)
		{
			group = JProtocol::instance()->config().group("priority");
			p->priority.insert(Presence::Available, group.value("online", 3));
			p->priority.insert(Presence::Chat, group.value("ffchat", 3));
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
		if (presence == Presence::Unavailable) {
			p->client->disconnect();
			p->account->endChangeStatus(presence);
		}
	}
	
	void JConnection::setAvatar(const QString &hex)
	{
		if (p->avatarHash != hex) {
			p->avatarHash = hex;
			ConfigGroup general = p->account->config("general");
			general.setValue("avatarhash", p->avatarHash);
			general.sync();
			
			StanzaExtensionList *extensions = (StanzaExtensionList *) &(p->client->presence().extensions());
			StanzaExtensionList::iterator it = extensions->begin();
			StanzaExtensionList::iterator it2;
			while (it != extensions->end()) {
				it2 = it++;
				if ((*it2)->extensionType() == ExtVCardUpdate) {
					delete (*it2);
					extensions->erase(it2);
				}
			}
			p->client->addPresenceExtension(new VCardUpdate(p->avatarHash.toStdString()));
			p->client->setPresence();
		}
	}
}
