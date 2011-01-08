/****************************************************************************
 *  jaccount.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include "../jprotocol.h"
#include "muc/jmucmanager.h"
#include "muc/jmucuser.h"
#include <qutim/systeminfo.h>
#include <qutim/passworddialog.h>
#include <qutim/debug.h>
#include <qutim/event.h>
#include <qutim/dataforms.h>
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

namespace Jabber {

class JPasswordValidator : public QValidator
{
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
	q->setAccountStatus(status);
	client.setPresence(JStatus::statusToPresence(status), status.text());
}

void JAccountPrivate::setPresence(jreen::Presence presence)
{
	Q_Q(JAccount);
	Status now = status;
	now.setType(JStatus::presenceToStatus(presence.subtype()));
	now.setText(presence.status());
	q->setAccountStatus(now);
	if(presence.subtype() == jreen::Presence::Unavailable)
		client.disconnectFromServer(false);
}

void JAccountPrivate::onConnected()
{
	Q_Q(JAccount);
	applyStatus(status);
	vCardManager->fetchVCard(q->id());
	conferenceManager->syncBookmarks();
	q->resetGroupChatManager(conferenceManager->bookmarkManager());
}

void JAccountPrivate::onModuleLoaded(int i)
{
	qDebug() << Q_FUNC_INFO << loadedModules << i << sender();
	loadedModules |= i;
	if (loadedModules == 3)
		onConnected();
}

void JAccountPrivate::onDisconnected()
{
	Q_Q(JAccount);
	Status now = q->status();
	now.setType(Status::Offline);	
	q->setAccountStatus(now);
	q->resetGroupChatManager(0);
	loadedModules = 0;
}

void JAccountPrivate::initExtensions(const QSet<QString> &features)
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
	Account::setStatus(Status::instance(Status::Offline, "jabber"));

	d->loadedModules = 0;
	d->roster = new JRoster(this);
	d->privacyManager = new PrivacyManager(&d->client);
	jreen::Capabilities::Ptr caps = d->client.presence().findExtension<jreen::Capabilities>();
	caps->setNode(QLatin1String("http://qutim.org/"));
	d->privateXml = new jreen::PrivateXml(&d->client);
	d->pubSubManager = new jreen::PubSub::Manager(&d->client);
	d->conferenceManager = new JMUCManager(this, this);
	d->messageSessionManager = new JMessageSessionManager(this);
	d->vCardManager = new JVCardManager(this);
	d->softwareDetection = new JSoftwareDetection(this);

	d->client.presence().addExtension(new VCardUpdate(QString()));
	loadSettings();

	jreen::Disco *disco = d->client.disco();
	disco->setSoftwareVersion(QLatin1String("qutIM"),
							  qutimVersionStr(),
							  SystemInfo::getName());

	disco->addIdentity(jreen::Disco::Identity(QLatin1String("client"),
											  QLatin1String("type"),
											  QLatin1String("qutIM"),
											  QLatin1String("en")));
	QString qutim = tr("qutIM", "Local qutIM's name");
	QString lang = tr("en", "Default language");
	if(qutim != QLatin1String("qutIM") && lang != QLatin1String("en"))
		disco->addIdentity(jreen::Disco::Identity(QLatin1String("client"), QLatin1String("type"),qutim,lang));
	
	connect(d->roster, SIGNAL(loaded()), &d->signalMapper, SLOT(map()));
	connect(d->privacyManager, SIGNAL(listsReceived()), &d->signalMapper, SLOT(map()));
	d->signalMapper.setMapping(d->roster, 1);
	d->signalMapper.setMapping(d->privacyManager, 2);
	connect(&d->client, SIGNAL(connected()), d->privacyManager, SLOT(request()));
	connect(&d->signalMapper, SIGNAL(mapped(int)), d, SLOT(onModuleLoaded(int)));
	//	connect(d->roster, SIGNAL(loaded()), d, SLOT(onConnected()));
	//	connect(&d->client,SIGNAL(connected()), d, SLOT(onConnected()));
	
	connect(&d->client, SIGNAL(disconnected()),
			d,SLOT(onDisconnected()));
	connect(&d->client, SIGNAL(serverFeaturesReceived(QSet<QString>)),
			d,SLOT(initExtensions(QSet<QString>)));
	connect(d->conferenceManager.data(), SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			this, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
	
	d->params.addItem<jreen::Client>(&d->client);
	d->params.addItem<jreen::PubSub::Manager>(d->pubSubManager);
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
	jreen::JID jid = unitId;
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
	d->client.setPassword(general.value("passwd", QString(), Config::Crypted));
	d->client.setPort(general.value("port", 5222));
	d->keepStatus = general.value("keepstatus", true);
	d->nick = general.value("nick", id());
	jreen::VCardUpdate::Ptr update = d->client.presence().findExtension<jreen::VCardUpdate>();
	update->setPhotoHash(general.value("photoHash", QString()));

	jreen::JID jid(id());
	jid.setResource(general.value("resource",QLatin1String("qutIM/jreen")));
	d->client.setJID(jid);

	general.endGroup();
}

void JAccount::setPasswd(const QString &passwd)
{
	Q_D(JAccount);
	config().group("general").setValue("passwd",passwd, Config::Crypted);
	d->passwd = passwd;
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

const QString &JAccount::password(bool *ok)
{
	Q_D(JAccount);
	if (ok)
		*ok = true;
	if (d->passwd.isEmpty()) {
		if (ok)
			*ok = false;
		PasswordDialog *dialog = PasswordDialog::request(this);
		JPasswordValidator *validator = new JPasswordValidator();
		dialog->setValidator(validator);
		if (dialog->exec() == PasswordDialog::Accepted) {
			if (ok)
				*ok = true;
			d->passwd = dialog->password();
			if (dialog->remember()) {
				config().group("general").setValue("passwd", d->passwd, Config::Crypted);
				config().sync();
			}
		}
		delete validator;
		delete dialog;
	}
	return d->passwd;
}

JMessageSessionManager *JAccount::messageSessionManager() const
{
	return d_func()->messageSessionManager;
}

jreen::Client *JAccount::client() const
{
	//it may be dangerous
	return const_cast<jreen::Client*>(&d_func()->client);
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

jreen::PrivateXml *JAccount::privateXml()
{
	return d_func()->privateXml;
}

jreen::PubSub::Manager *JAccount::pubSubManager()
{
	return d_func()->pubSubManager;
}

void JAccount::setStatus(Status status)
{
	Q_D(JAccount);
	Status old = this->status();

	if(old.type() == Status::Offline && status.type() != Status::Offline) {
		if(d->client.password().isEmpty()) {
			bool ok;
			d->client.setPassword(password(&ok));
			if(!ok)
				return;
		}
		d->client.connectToServer();
		d->status = status;
		status.setType(Status::Connecting);
		setAccountStatus(status);
	} else if(status.type() == Status::Offline) {
		bool force = old.type() == Status::Connecting;
		if (force) {
			status.setType(Status::Offline);
			setAccountStatus(status);
		}
		d->client.disconnectFromServer(true);
	} else if(old.type() != Status::Offline && old.type() != Status::Connecting) {
		d->applyStatus(status);
		//		d->client.setPresence(JStatus::statusToPresence(status), status.text());
	}
}

void JAccount::setAccountStatus(Status status)
{
	Account::setStatus(status);
	emit statusChanged(status,this->status());
}

QString JAccount::getAvatarPath()
{
	return QString("%1/avatars/%2")
			.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
			.arg(protocol()->id());
}

bool JAccount::event(QEvent *ev)
{
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
	const jreen::Disco::IdentityList identities = d->client.serverIdentities();
	bool ok = false;
	for (int i = 0; !ok && i < identities.size(); i++) {
		const jreen::Disco::Identity &identity = identities[i];
		ok |= (identity.category == category && identity.type == type);
	}
	return ok;
}

QString JAccount::identity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	const jreen::Disco::IdentityList identities = d->client.serverIdentities();
	for (int i = 0; i < identities.size(); i++) {
		const jreen::Disco::Identity &identity = identities[i];
		if (identity.category == category && identity.type == type)
			return identity.name;
	}
	return QString();
}

} // Jabber namespace
