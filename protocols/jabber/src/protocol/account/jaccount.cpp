/****************************************************************************
 *  jaccount.cpp
 *
 *  Copyright (c) 2010-2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "jaccount_p.h"
#include "roster/jroster.h"
#include "roster/jcontact.h"
#include "roster/jcontactresource.h"
#include "roster/jmessagehandler.h"
#include "connection/jserverdiscoinfo.h"
#include "servicediscovery/jservicebrowser.h"
#include "servicediscovery/jservicediscovery.h"
#include "vcard/jinforequest.h"
#include "../jprotocol.h"
#include "muc/jmucmanager.h"
#include "muc/jmucuser.h"
#include <qutim/systeminfo.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <qutim/dataforms.h>
#include <qutim/libqutim_version.h>
#include <jreen/jid.h>
#include <jreen/dataform.h>
#include <jreen/disco.h>
#include <jreen/iq.h>
#include <jreen/vcard.h>
#include <jreen/capabilities.h>
#include <jreen/vcardupdate.h>
#include <qutim/systeminfo.h>
#include "roster/jsoftwaredetection.h"
#include <jreen/pubsubmanager.h>
#include <jreen/connectionbosh.h>
#include <qutim/debug.h>

namespace Jabber {

class JPasswordValidator : public QValidator
{
public:
	JPasswordValidator(QObject *parent = 0) : QValidator(parent) {}
	
	State validate(QString &input, int &pos) const
	{
		Q_UNUSED(pos);
		if (input.isEmpty())
			return Intermediate;
		else
			return Acceptable;
	}
};

void JAccountPrivate::applyStatus(const Status &status)
{
	Q_Q(JAccount);
	QString invisible = QLatin1String("invisible");
	if (status.type() == Status::Invisible) {
		if (privacyManager->activeList() != invisible) {
			if (!privacyManager->lists().contains(invisible)) {
				PrivacyItem item;
				item.setAction(PrivacyItem::Deny);
				item.setOrder(1);
				item.setStanzaTypes(PrivacyItem::PresenceOut);
				privacyManager->setList(invisible, QList<PrivacyItem>() << item);
			}
			client.setPresence(Presence::Unavailable);
			privacyManager->setActiveList(invisible);
		}
	} else {
		if (privacyManager->activeList() == invisible)
			privacyManager->desetActiveList();
	}
	client.setPresence(JStatus::statusToPresence(status), status.text(), priority);
	q->setAccountStatus(status);
}

void JAccountPrivate::setPresence(Jreen::Presence presence)
{
	Q_Q(JAccount);
	Status now = JStatus::presenceToStatus(presence.subtype());
	now.setText(presence.status());
	q->setAccountStatus(now);
	if(presence.subtype() == Jreen::Presence::Unavailable)
		client.disconnectFromServer(false);
}

void JAccountPrivate::_q_connected()
{
	Q_Q(JAccount);
	applyStatus(status);
	vCardManager->fetchVCard(q->id());
	conferenceManager->syncBookmarks();
	q->resetGroupChatManager(conferenceManager->bookmarkManager());	
	client.setPingInterval(q->config().group("general").value("pingInterval", 30000));
}

void JAccountPrivate::_q_on_password_finished(int result)
{
	Q_Q(JAccount);
	Q_ASSERT(q->sender() == passwordDialog);
	passwordDialog->deleteLater();
	if (result != PasswordDialog::Accepted)
		return;
	if (passwordDialog->remember()) {
		Config cfg = q->config(QLatin1String("general"));
		cfg.setValue("passwd", passwordDialog->password(), Config::Crypted);
	}
	status = passwordDialog->property("status").value<Status>();
	client.setPassword(passwordDialog->password());
	client.connectToServer();
	q->setAccountStatus(Status::instance(Status::Connecting, "jabber"));
}

void JAccountPrivate::_q_on_module_loaded(int i)
{
	qDebug() << Q_FUNC_INFO << loadedModules << i << q_func()->sender();
	loadedModules |= i;
	if (loadedModules == 3)
		_q_connected();
}

void JAccountPrivate::_q_disconnected(Jreen::Client::DisconnectReason reason)
{
	Q_Q(JAccount);
	Status s = Status::instance(Status::Offline, "jabber");

	switch(reason) {
	case Client::User:
		s.setChangeReason(Status::ByUser);
		break;
	case Client::AuthorizationError: {
		s.setChangeReason(Status::ByAuthorizationFailed);
		//q->setPasswd(QString());
		break;
	}
	case Client::HostUnknown:
	case Client::ItemNotFound:
	case Client::SystemShutdown:
		s.setChangeReason(Status::ByFatalError);
		break;
	case Client::RemoteStreamError:
	case Client::RemoteConnectionFailed:
	case Client::InternalServerError:
	case Client::Conflict:
	case Client::Unknown:
		s.setChangeReason(Status::ByNetworkError);
		break;
	}

	status = s;
	q->setAccountStatus(s);
	q->resetGroupChatManager(0);
	loadedModules = 0;
}

void JAccountPrivate::_q_init_extensions(const QSet<QString> &features)
{
	//Q_Q(JAccount);
	debug() << "received features list" << features;
	//	foreach (const ObjectGenerator *gen, ObjectGenerator::module<JabberExtension>()) {
	//		if (JabberExtension *ext = gen->generate<JabberExtension>()) {
	//			debug() << "init ext" << ext;
	//			extensions.append(ext);
	//			ext->init(q,params);
	//		}
	//	}
	roster->load();
}

JAccount::JAccount(const QString &id) :
	Account(id, JProtocol::instance()),
	d_ptr(new JAccountPrivate(this))
{
	Q_D(JAccount);
	connect(&d->client, SIGNAL(disconnected(Jreen::Client::DisconnectReason)),
			this, SLOT(_q_disconnected(Jreen::Client::DisconnectReason)));
	connect(&d->client, SIGNAL(serverFeaturesReceived(QSet<QString>)),
			this ,SLOT(_q_init_extensions(QSet<QString>)));

	Account::setStatus(Status::instance(Status::Offline, "jabber"));
	d->loadedModules = 0;
	d->roster = new JRoster(this);
	d->privacyManager = new PrivacyManager(&d->client);
	Jreen::Capabilities::Ptr caps = d->client.presence().payload<Jreen::Capabilities>();
	caps->setNode(QLatin1String("http://qutim.org/"));
	d->privateXml = new Jreen::PrivateXml(&d->client);
	d->pubSubManager = new Jreen::PubSub::Manager(&d->client);
	d->conferenceManager = new JMUCManager(this, this);
	d->messageSessionManager = new JMessageSessionManager(this);
	d->vCardManager = new JVCardManager(this);
	d->softwareDetection = new JSoftwareDetection(this);

	d->client.presence().addExtension(new VCardUpdate());

	Jreen::Disco *disco = d->client.disco();
	disco->setSoftwareVersion(QLatin1String("qutIM"),
							  versionString(),
							  SystemInfo::getFullName());

	disco->addIdentity(Jreen::Disco::Identity(QLatin1String("client"),
											  QLatin1String("type"),
											  QLatin1String("qutIM"),
											  QLatin1String("en")));
	QString qutim = tr("qutIM", "Local qutIM's name");
	QString lang = tr("en", "Default language");
	if(qutim != QLatin1String("qutIM") && lang != QLatin1String("en"))
		disco->addIdentity(Jreen::Disco::Identity(QLatin1String("client"), QLatin1String("type"),qutim,lang));
	
	connect(d->roster, SIGNAL(loaded()), &d->signalMapper, SLOT(map()));
	connect(d->privacyManager, SIGNAL(listsReceived()), &d->signalMapper, SLOT(map()));
	d->signalMapper.setMapping(d->roster, 1);
	d->signalMapper.setMapping(d->privacyManager, 2);
	connect(&d->client, SIGNAL(connected()), d->privacyManager, SLOT(request()));
	connect(&d->signalMapper, SIGNAL(mapped(int)), this, SLOT(_q_on_module_loaded(int)));
	//	connect(d->roster, SIGNAL(loaded()), d, SLOT(onConnected()));
	//	connect(&d->client,SIGNAL(connected()), d, SLOT(onConnected()));
	
	d->roster->loadFromStorage();

	connect(d->conferenceManager.data(), SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			this, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
	
	d->params.addItem<Jreen::Client>(&d->client);
	d->params.addItem<Jreen::PubSub::Manager>(d->pubSubManager);
	//	d->params.addItem<Adhoc>(p->adhoc);
	//	d->params.addItem<VCardManager>(p->vCardManager->manager());
	//	d->params.addItem<SIManager>(p->siManager);

	//	d->softwareDetection = new JSoftwareDetection(p->account, p->params);

	foreach (const ObjectGenerator *gen, ObjectGenerator::module<JabberExtension>()) {
		if (JabberExtension *ext = gen->generate<JabberExtension>()) {
			d->extensions.append(ext);
			ext->init(this, d->params);
		}
	}
	loadSettings();
}

JAccount::~JAccount()
{
}

ChatUnit *JAccount::getUnitForSession(ChatUnit *unit)
{
	if (qobject_cast<JContactResource*>(unit) && !qobject_cast<JMUCUser*>(unit))
		unit = unit->upperUnit();
	return unit;
}

ChatUnit *JAccount::getUnit(const QString &unitId, bool create)
{
	Q_D(JAccount);
	ChatUnit *unit = 0;
	Jreen::JID jid = unitId;
	if (!!(unit = d->conferenceManager->muc(jid)))
		return unit;
	return d->roster->contact(jid, create);
	return 0;
}

void JAccount::loadSettings()
{
	Q_D(JAccount);
	Config general = config();
	general.beginGroup("general");

	d->priority = general.value("priority", 15);
	d->keepStatus = general.value("keepstatus", true);
	d->nick = general.value("nick", id());
	if (general.hasChildKey("photoHash")) {
		Jreen::VCardUpdate::Ptr update = d->client.presence().payload<Jreen::VCardUpdate>();
		update->setPhotoHash(general.value("photoHash", QString()));
	}

	Jreen::JID jid(id());
	jid.setResource(general.value("resource",QLatin1String("qutIM/Jreen")));
	general.beginGroup("bosh");
	if (general.value("use", false)) {
		QString host = general.value("host", jid.domain());
		int port = general.value("port", 5280);
		d->client.setConnection(new ConnectionBOSH(host, port));
	}
	general.endGroup();
	d->client.setJID(jid);
	d->client.setPassword(general.value("passwd", QString(), Config::Crypted));
	if(!general.value("autoDetect",true)) {
		d->client.setPort(general.value("port", 5222));
		d->client.setServer(general.value("server",d->client.server()));
	}

	general.endGroup();
}

void JAccount::setPasswd(const QString &passwd)
{
	Q_D(JAccount);
	config().group("general").setValue("passwd",passwd, Config::Crypted);
	d->client.setPassword(passwd);
	config().sync();
}

JRoster *JAccount::roster() const
{
	return d_func()->roster;
}

JServiceDiscovery *JAccount::discoManager()
{
	Q_D(JAccount);
	if (!d->discoManager)
		d->discoManager = new JServiceDiscovery(this);
	return d->discoManager;
}

QString JAccount::name() const
{
	return d_func()->nick;
}

void JAccount::setNick(const QString &nick)
{
	Q_D(JAccount);
	Config general = config("general");
	general.setValue("nick", nick);
	general.sync();
	QString previous = d->nick;
	d->nick = nick;
	emit nameChanged(nick, previous);
}

QString JAccount::getPassword() const
{
	return d_func()->client.password();
}

QString JAccount::password(bool *ok)
{
	Q_D(JAccount);
	if (ok)
		*ok = true;
	if (d->client.password().isEmpty()) {
		if (ok)
			*ok = false;
		if (!d->passwordDialog) {
			d->passwordDialog = PasswordDialog::request(this);
			JPasswordValidator *validator = new JPasswordValidator(d->passwordDialog);
			connect(d->passwordDialog, SIGNAL(finished(int)), SLOT(_q_on_password_finished(int)));
			d->passwordDialog->setValidator(validator);
		}
		if (d->passwordDialog->exec() == PasswordDialog::Accepted) {
			if (ok)
				*ok = true;
			d->client.setPassword(d->passwordDialog->password());
			if (d->passwordDialog->remember()) {
				config().group("general").setValue("passwd", d->client.password(), Config::Crypted);
				config().sync();
			}
			d->passwordDialog->deleteLater();
		}
	}
	return d->client.password();
}

JMessageSessionManager *JAccount::messageSessionManager() const
{
	return d_func()->messageSessionManager;
}

Jreen::Client *JAccount::client() const
{
	//it may be dangerous
	return const_cast<Jreen::Client*>(&d_func()->client);
}

JSoftwareDetection *JAccount::softwareDetection() const
{
	return d_func()->softwareDetection;
}

JVCardManager *JAccount::vCardManager() const
{
	return d_func()->vCardManager;
}

JMUCManager *JAccount::conferenceManager()
{
	return d_func()->conferenceManager;
}

Jreen::PrivateXml *JAccount::privateXml()
{
	return d_func()->privateXml;
}

Jreen::PrivacyManager *JAccount::privacyManager()
{
	return d_func()->privacyManager;
}

Jreen::PubSub::Manager *JAccount::pubSubManager()
{
	return d_func()->pubSubManager;
}

void JAccount::setStatus(Status status)
{
	Q_D(JAccount);
	Status old = this->status();

	if(old.type() == Status::Offline && status.type() != Status::Offline) {
		if (d->passwordDialog) {
			/* nothing */
		} else if(d->client.password().isEmpty()) {
			d->passwordDialog = PasswordDialog::request(this);
			d->passwordDialog->setProperty("status", qVariantFromValue(status));
			JPasswordValidator *validator = new JPasswordValidator(d->passwordDialog);
			connect(d->passwordDialog, SIGNAL(finished(int)), SLOT(_q_on_password_finished(int)));
			d->passwordDialog->setValidator(validator);
		} else {
			d->client.connectToServer();
			d->status = status;
			setAccountStatus(Status::instance(Status::Connecting, "jabber"));
		}
	} else if(status.type() == Status::Offline) {
		bool force = old.type() == Status::Connecting;
		if (force)
			setAccountStatus(Status::instance(Status::Offline, "jabber"));
		d->client.disconnectFromServer(true);
	} else if(old.type() != Status::Offline && old.type() != Status::Connecting) {
		d->applyStatus(status);
		//		d->client.setPresence(JStatus::statusToPresence(status), status.text());
	}
}

void JAccount::setAccountStatus(Status status)
{
	Q_D(JAccount);
	if (status != Status::Connecting && status != Status::Offline)
		d->conferenceManager->setPresenceToRooms(d->client.presence());
	Account::setStatus(status);
}

QString JAccount::getAvatarPath()
{
	return QString("%1/avatars/%2")
			.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
			.arg(protocol()->id());
}

bool JAccount::event(QEvent *ev)
{
	if (ev->type() == InfoRequestCheckSupportEvent::eventType()) {
		Status::Type status = Account::status().type();
		if (status >= Status::Online && status <= Status::Invisible) {
			InfoRequestCheckSupportEvent *event = static_cast<InfoRequestCheckSupportEvent*>(ev);
			event->setSupportType(InfoRequestCheckSupportEvent::ReadWrite);
			event->accept();
		} else {
			ev->ignore();
		}
		return true;
	} else if (ev->type() == InfoRequestEvent::eventType()) {
		InfoRequestEvent *event = static_cast<InfoRequestEvent*>(ev);
		event->setRequest(new JInfoRequest(vCardManager(), id()));
		event->accept();
		return true;
	} else if (ev->type() == InfoItemUpdatedEvent::eventType()) {
		InfoItemUpdatedEvent *event = static_cast<InfoItemUpdatedEvent*>(ev);
		VCard::Ptr vcard = JInfoRequest::convert(event->infoItem());
		vCardManager()->storeVCard(vcard);
		event->accept();
		return true;
	}
	return Account::event(ev);
}

QSet<QString> JAccount::features() const
{
	return d_func()->client.serverFeatures();
}

bool JAccount::checkFeature(const QString &feature) const
{
	return d_func()->client.serverFeatures().contains(feature);
}

bool JAccount::checkIdentity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	const Jreen::Disco::IdentityList identities = d->client.serverIdentities();
	bool ok = false;
	for (int i = 0; !ok && i < identities.size(); i++) {
		const Jreen::Disco::Identity &identity = identities[i];
		ok |= (identity.category == category && identity.type == type);
	}
	return ok;
}

QString JAccount::identity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	const Jreen::Disco::IdentityList identities = d->client.serverIdentities();
	for (int i = 0; i < identities.size(); i++) {
		const Jreen::Disco::Identity &identity = identities[i];
		if (identity.category == category && identity.type == type)
			return identity.name;
	}
	return QString();
}

} // Jabber namespace

#include "jaccount.moc"
