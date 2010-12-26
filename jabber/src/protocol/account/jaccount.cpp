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
#include <qutim/systeminfo.h>
#include "roster/jsoftwaredetection.h"

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

void JAccountPrivate::setPresence(jreen::Presence presence)
{
	Q_Q(JAccount);
	Status now = q->status();
	status = presence.subtype();
	now.setType(JStatus::presenceToStatus(status));
	now.setText(presence.status());
	q->setAccountStatus(now);
	if(status == jreen::Presence::Unavailable)
		client.disconnectFromServer(false);
}

void JAccountPrivate::onConnected()
{
	Q_Q(JAccount);
	Status s = q->status();
	s.setType(Status::Online);
	q->setAccountStatus(s);
	client.setPresence(status,s.text());
}

void JAccountPrivate::onDisconnected()
{
	Q_Q(JAccount);
	Status now = q->status();
	now.setType(Status::Offline);	
	q->setAccountStatus(now);
}

void JAccountPrivate::initExtensions(const QSet<QString> &features)
{
	Q_Q(JAccount);
	debug() << "received features list";
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
	p = d; //for dead code
	Account::setStatus(Status::instance(Status::Offline, "jabber"));

	jreen::JID jid(id);
	jid.setResource(QLatin1String("jreen(qutIM)"));
	d->client.setJID(jid);
	d->roster = new JRoster(this);
	d->messageSessionManager = new JMessageSessionManager(this);
	d->vCardManager = new JVCardManager(this);
	new JSoftwareDetection(this);
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

	connect(&d->client,SIGNAL(connected()),
			d,SLOT(onConnected()));
	connect(&d->client,SIGNAL(disconnected()),
			d,SLOT(onDisconnected()));
	connect(&d->client, SIGNAL(serverFeaturesReceived(QSet<QString>)),
			d,SLOT(initExtensions(QSet<QString>)));
	
	d->params.addItem<jreen::Client>(&d->client);
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
	//	ChatUnit *unit = 0;
	//	if (!!(unit = d->conferenceManager->muc(unitId)))
	//		return unit;
	return d->roster->contact(unitId, create);
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
	if (!p->discoManager)
		p->discoManager = new JServiceDiscovery(this);
	return p->discoManager;
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

JVCardManager *JAccount::vCardManager() const
{
	return d_func()->vCardManager;
}

JMUCManager *JAccount::conferenceManager()
{
	return d_func()->conferenceManager;
}

void JAccount::setStatus(Status status)
{
	Q_D(JAccount);
	Status old = this->status();

	if(old.type() == Status::Offline && status.type() != Status::Offline) {
		d->client.connectToServer();
		d->status = JStatus::statusToPresence(status);
		status.setType(Status::Connecting);
		setAccountStatus(status);
	} else if(status.type() == Status::Offline) {
		bool force = old.type() == Status::Connecting;
		if(force) {
			status.setType(Status::Offline);
			setAccountStatus(status);
		}
		d->client.disconnectFromServer(true);
	} else if(old.type() != Status::Offline && old.type() != Status::Connecting) {
		d->client.setPresence(JStatus::statusToPresence(status),
							  status.text());
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

QVariantList JAccountPrivate::toVariant(const QList<JBookmark> &list)
{
	QVariantList items;
	foreach (const JBookmark &bookmark, list) {
		QVariantMap item;
		QString name = bookmark.name.isEmpty() ? bookmark.conference : bookmark.name;
		item.insert("name",name);
		QVariantMap data;
		data.insert(QT_TRANSLATE_NOOP("Jabber", "Conference"),bookmark.conference);
		data.insert(QT_TRANSLATE_NOOP("Jabber", "Nick"),bookmark.nick);
		if (bookmark.autojoin)
			data.insert(QT_TRANSLATE_NOOP("Jabber", "Autojoin"),(QT_TRANSLATE_NOOP("Jabber", "Yes")).toString());
		item.insert("fields",data);
		items.append(item);
	}
	return items;
}

bool JAccount::event(QEvent *ev)
{
	Q_D(JAccount);
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

bool JAccount::checkFeature(const std::string &feature) const
{
	return d_func()->client.serverFeatures().contains(QString::fromStdString(feature));
}

bool JAccount::checkIdentity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	Identities::const_iterator catItr = d->identities.find(category);
	return catItr == d->identities.constEnd() ? false : catItr->contains(type);
}

bool JAccount::checkIdentity(const std::string &category, const std::string &type) const
{
	return checkIdentity(QString::fromStdString(category), QString::fromStdString(type));
}

QString JAccount::identity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	Identities::const_iterator catItr = d->identities.find(category);
	return catItr == d->identities.constEnd() ? QString() : catItr->value(type);
}

std::string JAccount::identity(const std::string &category, const std::string &type) const
{
	return identity(QString::fromStdString(category), QString::fromStdString(type)).toStdString();
}

} // Jabber namespace
