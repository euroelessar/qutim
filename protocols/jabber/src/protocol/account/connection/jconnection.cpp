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
//#include "jconnection.h"
//#include "../../jprotocol.h"
//#include "../jaccount.h"
//#include "../roster/jsoftwaredetection.h"
//#include "sdk/jabber.h"
//#include "../vcard/jvcardmanager.h"
//#include <gloox/adhoc.h>
//#include <qutim/systeminfo.h>
//#include <gloox/capabilities.h>
//#include <gloox/receipt.h>
//#include <gloox/vcardupdate.h>
//#include <gloox/simanager.h>
//#include <qutim/debug.h>

//namespace Jabber
//{
//	using namespace gloox;
//struct JConnectionPrivate
//{
//	JConnectionPrivate() {}
//	~JConnectionPrivate() {}
//	JabberParams params;
//	JAccount *account;
//	Client *client;
//	Adhoc *adhoc;
//	JVCardManager *vCardManager;
//	JConnectionTCPBase *connection;
//	JSoftwareDetection *softwareDetection;
//	SIManager *siManager;
//	QNetworkProxy proxy;
//	QString resource;
//	bool autoPriority;
//	QMap<Presence::PresenceType, int> priority;
//	QString password;
//	QList<JabberExtension *> extensions;
//	QString avatarHash;
//};

//JConnection::JConnection(JAccount *account) : p(new JConnectionPrivate)
//{
////	p->account = account;
////	p->password = QString();
////	p->softwareDetection = 0;
////	JID jid = JID(account->id().toStdString());
////	p->client = new Client(jid, p->password.toStdString());
////	p->adhoc = new Adhoc(p->client);
////	p->vCardManager = new JVCardManager(p->account, p->client);
////	p->connection = new JConnectionTCPBase(p->client);
////	p->siManager = new SIManager(p->client, true);
////	loadSettings();

////	p->client->registerPayload(new Receipt(Receipt::Invalid));
////	p->client->registerPayload(new VCardUpdate());
////	p->client->addPresenceExtension(new VCardUpdate(p->avatarHash.toStdString()));
////	Capabilities *caps = new Capabilities(p->client->disco());
////	caps->setNode("http://qutim.org");
////	p->client->addPresenceExtension(caps);

////	p->client->setConnectionImpl(p->connection);
////	qDebug() << p->client->connectionImpl() << p->connection;
////	p->client->disco()->setVersion("qutIM", qutimVersionStr(),
////								   SystemInfo::getFullName().toStdString());
////	p->client->disco()->setIdentity("client", "pc", "qutIM");
////	p->client->disco()->addFeature("jabber:iq:roster");
////	p->client->registerPresenceHandler(this);

////	// XEP-0232
////	DataForm *form = new DataForm(TypeResult);
////	form->addField(DataFormField::TypeHidden, "FORM_TYPE", "urn:xmpp:dataforms:softwareinfo");
////	form->addField(DataFormField::TypeNone, "os", SystemInfo::getName().toStdString());
////	form->addField(DataFormField::TypeNone, "os_version", SystemInfo::getVersion().toStdString());
////	form->addField(DataFormField::TypeNone, "software", "qutIM");
////	form->addField(DataFormField::TypeNone, "software_version", qutimVersionStr());
////	p->client->disco()->setForm(form);
////	p->client->logInstance().registerLogHandler(LogLevelDebug, LogAreaAllClasses, this);

////	p->client->disco()->addFeature("http://jabber.org/protocol/chatstates");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/bytestreams");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/si");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/si/profile/file-transfer");
////	p->client->disco()->addFeature("http://jabber.org/protocol/disco#info");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/commands");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/rosterx");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/muc");
////	p->client->disco()->addFeature("jabber:x:data");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/activity+notify");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/mood+notify");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/tune+notify");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/physloc+notify");
////	//		p->client->disco()->addFeature("http://jabber.org/protocol/geoloc+notify");
////	//		p->client->disco()->addFeature("http://www.xmpp.org/extensions/xep-0084.html#ns-metadata+notify");
////	p->client->disco()->addFeature("urn:xmpp:receipts");
////	p->client->disco()->addFeature("http://jabber.org/protocol/xhtml-im");
////	//		p->client->disco()->addFeature("urn:xmpp:tmp:sxe");
////	//		p->client->disco()->addFeature("http://www.w3.org/2000/svg");
//}

//JConnection::~JConnection()
//{
//	qDeleteAll(p->extensions);
//}

//Client *JConnection::client()
//{
//	return p->client;
//}

//Adhoc *JConnection::adhoc()
//{
//	return p->adhoc;
//}

//JVCardManager *JConnection::vCardManager()
//{
//	return p->vCardManager;
//}

//SIManager *JConnection::siManager()
//{
//	return p->siManager;
//}

//JConnectionBase *JConnection::connection()
//{
//	return p->connection;
//}

//JSoftwareDetection *JConnection::softwareDetection()
//{
//	return p->softwareDetection;
//}

//JabberParams JConnection::params() const
//{
//	return p->params;
//}

//void JConnection::initExtensions()
//{
//	p->params.addItem<Jreen::Client>(p->account->client());
////	p->params.addItem<Adhoc>(p->adhoc);
////	p->params.addItem<VCardManager>(p->vCardManager->manager());
////	p->params.addItem<SIManager>(p->siManager);

////	p->softwareDetection = new JSoftwareDetection(p->account, p->params);

//	foreach (const ObjectGenerator *gen, ObjectGenerator::module<JabberExtension>()) {
//		if (JabberExtension *ext = gen->generate<JabberExtension>()) {
//			p->extensions.append(ext);
//			ext->init(p->account, p->params);
//		}
//	}
//}

//void JConnection::handlePresence(const Presence &presence)
//{
//	//if (presence.from() == p->client->jid())
//	//	p->account->endChangeStatus(presence.presence());
//}

//void JConnection::loadSettings()
//{
//	// TODO: FIXME: Protocol settings must be fallback for account everywhere %)
//	QString defaultResource = JProtocol::instance()->config().group("general").value("defaultresource", QString("qutIM"));
//	Config group = p->account->config().group("general");
//	p->resource = group.value("resource", defaultResource);
//	p->avatarHash = group.value("avatarhash", QString());
//	p->client->setResource(p->resource.toStdString());
//	p->priority.clear();
//	p->priority.insert(Presence::Invalid, group.value("priority", 3));
//	p->autoPriority = group.value("autopriority",true);
//	if (p->autoPriority)
//	{
//		group = JProtocol::instance()->config().group("priority");
//		p->priority.insert(Presence::Available, group.value("online", 3));
//		p->priority.insert(Presence::Chat, group.value("ffchat", 3));
//		p->priority.insert(Presence::Away, group.value("away", 2));
//		p->priority.insert(Presence::XA, group.value("na", 1));
//		p->priority.insert(Presence::DND, group.value("dnd", -1));
//	}
//	//proxy settings
//	group = p->account->config().group("proxy");
//	QNetworkProxy::ProxyType t = QNetworkProxy::DefaultProxy;;
//	switch (group.value("type", 0)) {
//	case 0: // Default proxy
//		t = QNetworkProxy::DefaultProxy;
//		break;
//	case 1: // Socks5
//		t = QNetworkProxy::Socks5Proxy;
//		break;
//	case 2: // No proxy
//		t = QNetworkProxy::NoProxy;
//		break;
//	case 3: // Http
//		t = QNetworkProxy::HttpProxy;
//		break;
//	}
//	p->proxy.setType(t);
//	p->proxy.setHostName(group.value("hostname", QString()));
//	p->proxy.setPort(group.value("port", qint16()));
//	if (group.value("authorize", false)) {
//		p->proxy.setUser(group.value("user", QString()));
//		p->proxy.setPassword(group.value("password", QString()));
//	}
//	p->connection->setProxy(p->proxy);

//	//connect settings
//	group = p->account->config().group("connect");
//	TLSPolicy tls = TLSOptional;
//	switch (group.value("tls", 1)) {
//	case 0:
//		tls = TLSDisabled;
//		break;
//	case 1:
//		tls = TLSOptional;
//		break;
//	case 2:
//		tls = TLSRequired;
//		break;
//	}
//	p->client->setTls(tls);
//	p->client->setSasl(group.value("sasl", true));
//	p->client->setCompression(group.value("compression", true));
//	if (group.value("manualhost", false))
//		p->connection->setServer(group.value("server", QString::fromStdString(p->client->jid().server())).toStdString(),
//								 group.value("port", 5222));
//	else
//		p->connection->setServer(p->client->jid().server(), 5222);
//	p->connection->setUseDns(group.value("usedns", true));
//}

//void JConnection::setConnectionPresence(Presence::PresenceType presence)
//{
//	//if (presence == Presence::Unavailable) {
//	//	p->client->disconnect();
//	//	p->account->endChangeStatus(presence);
//	//} else {
//	//	if (p->password.isEmpty()) {
//	//		bool ok;
//	//		p->password = p->account->password(&ok);
//	//		if (ok)
//	//			p->client->setPassword(p->password.toStdString());
//	//		else
//	//			return;
//	//	}
//	//	p->client->setPresence(presence, p->autoPriority
//	//						   ? p->priority.value(presence) : p->priority.value(Presence::Invalid));
//	//	if (p->client->state() == StateDisconnected) {
//	//		p->client->setXmlLang(QLocale().name().section('_', 0, 0).toStdString());
//	//		p->client->connect(false);
//	//	}
//	//}
//}

//void JConnection::setAvatar(const QString &hex)
//{
//	if (p->avatarHash != hex) {
//		p->avatarHash = hex;
//		Config general = p->account->config("general");
//		general.setValue("avatarhash", p->avatarHash);
//		general.sync();

//		PayloadList *extensions = (PayloadList *) &(p->client->presence().extensions());
//		PayloadList::iterator it = extensions->begin();
//		PayloadList::iterator it2;
//		while (it != extensions->end()) {
//			it2 = it++;
//			if ((*it2)->payloadType() == ExtVCardUpdate) {
//				delete (*it2);
//				extensions->erase(it2);
//			}
//		}
//		p->client->addPresenceExtension(new VCardUpdate(p->avatarHash.toStdString()));
//		p->client->setPresence();
//	}
//}

//void JConnection::handleLog(LogLevel level, LogArea area, const std::string &message)
//{
//	qDebug() << QString::fromStdString(message);
//}
//}

