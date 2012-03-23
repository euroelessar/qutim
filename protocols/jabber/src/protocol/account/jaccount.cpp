/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "jaccount_p.h"
#include "roster/jroster.h"
#include "roster/jcontact.h"
#include "roster/jcontactresource.h"
#include "roster/jmessagehandler.h"
#include "connection/jserverdiscoinfo.h"
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
#include <jreen/directconnection.h>
#include <qutim/debug.h>
#include <qutim/networkproxy.h>

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
			client->setPresence(Presence::Unavailable);
			privacyManager->setActiveList(invisible);
		}
	} else {
		if (privacyManager->activeList() == invisible)
			privacyManager->desetActiveList();
	}
	client->setPresence(JStatus::statusToPresence(status), status.text(), priority);
	const Presence presence = client->presence();
	// We need this for peps
	Presence copy(presence.subtype(), client->jid().bareJID(), presence.status(), presence.priority());
	client->send(copy);
	q->setAccountStatus(status);
}

void JAccountPrivate::setPresence(Jreen::Presence presence)
{
	Q_Q(JAccount);
	Status now = JStatus::presenceToStatus(presence.subtype());
	now.setText(presence.status());
	q->setAccountStatus(now);
	if(presence.subtype() == Jreen::Presence::Unavailable)
		client->disconnectFromServer(false);
}

void JAccountPrivate::_q_connected()
{
	Q_Q(JAccount);
	applyStatus(status);
	conferenceManager.data()->syncBookmarks();
	q->resetGroupChatManager(conferenceManager.data()->bookmarkManager());	
	client->setPingInterval(q->config().group("general").value("pingInterval", 30000));
}

void JAccountPrivate::_q_on_password_finished(int result)
{
	Q_Q(JAccount);
	Q_ASSERT(q->sender() == passwordDialog.data());
	passwordDialog.data()->deleteLater();
	if (result != PasswordDialog::Accepted)
		return;
	if (passwordDialog.data()->remember()) {
		Config cfg = q->config(QLatin1String("general"));
		cfg.setValue("passwd", passwordDialog.data()->password(), Config::Crypted);
		parameters.insert(QLatin1String("password"), passwordDialog.data()->password());
		emit q->parametersChanged(parameters);
	}
	status = passwordDialog.data()->property("status").value<Status>();
	client->setPassword(passwordDialog.data()->password());
	client->connectToServer();
	q->setAccountStatus(Status::instance(Status::Connecting, "jabber"));
}

void JAccountPrivate::_q_on_module_loaded(int i)
{
	debug() << Q_FUNC_INFO << loadedModules << i << q_func()->sender();
	loadedModules |= i;
	if (loadedModules == 3)
		_q_connected();
}

void JAccountPrivate::_q_set_nick(const QString &newNick)
{
	Config general = q_ptr->config("general");
	general.setValue("nick", newNick);
	general.sync();
	QString previous = newNick;
	qSwap(nick, previous);
	emit q_ptr->nameChanged(nick, previous);
}

void JAccountPrivate::_q_disconnected(Jreen::Client::DisconnectReason reason)
{
	Q_Q(JAccount);
	Status s = Status::instance(Status::Offline, "jabber");

	switch(reason) {
	case Client::User:
		s.setProperty("changeReason", Status::ByUser);
		break;
	case Client::AuthorizationError: {
		s.setProperty("changeReason", Status::ByAuthorizationFailed);
		//q->setPasswd(QString());
		break;
	}
	case Client::HostUnknown:
	case Client::ItemNotFound:
	case Client::SystemShutdown:
	case Client::NoSupportedFuture:
	case Client::NoAuthorizationSupport:
	case Client::NoEncryptionSupport:
	case Client::NoCompressionSupport:
		s.setProperty("changeReason", Status::ByFatalError);
		break;
	case Client::RemoteStreamError:
	case Client::RemoteConnectionFailed:
	case Client::InternalServerError:
	case Client::Conflict:
	case Client::Unknown:
		s.setProperty("changeReason", Status::ByNetworkError);
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
	d->client.reset(new Client(id));
	connect(d->client.data(), SIGNAL(disconnected(Jreen::Client::DisconnectReason)),
			this, SLOT(_q_disconnected(Jreen::Client::DisconnectReason)));
	connect(d->client.data(), SIGNAL(serverFeaturesReceived(QSet<QString>)),
			this ,SLOT(_q_init_extensions(QSet<QString>)));

	Account::setStatus(Status::instance(Status::Offline, "jabber"));
	d->loadedModules = 0;
	d->roster = new JRoster(this);
	d->privacyManager = new PrivacyManager(d->client.data());
	Jreen::Capabilities::Ptr caps = d->client->presence().payload<Jreen::Capabilities>();
	caps->setNode(QLatin1String("http://qutim.org/"));
	d->privateXml = new Jreen::PrivateXml(d->client.data());
	d->pubSubManager = new Jreen::PubSub::Manager(d->client.data());
	d->conferenceManager = new JMUCManager(this, this);
	d->messageSessionManager = new JMessageSessionManager(this);
	d->softwareDetection = new JSoftwareDetection(this);
	d->client->presence().addExtension(new VCardUpdate());

	Jreen::Disco *disco = d->client->disco();
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
	connect(d->client.data(), SIGNAL(connected()), d->privacyManager, SLOT(request()));
	connect(&d->signalMapper, SIGNAL(mapped(int)), this, SLOT(_q_on_module_loaded(int)));
	//	connect(d->roster, SIGNAL(loaded()), d, SLOT(onConnected()));
	//	connect(d->client.data(),SIGNAL(connected()), d, SLOT(onConnected()));
	
	d->roster->loadFromStorage();

	connect(d->conferenceManager.data(), SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			this, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
	
//	d->params.addItem<Jreen::Client>(d->client);
//	d->params.addItem<Jreen::PubSub::Manager>(d->pubSubManager);
	//	d->params.addItem<Adhoc>(p->adhoc);
	//	d->params.addItem<VCardManager>(p->vCardManager->manager());
	//	d->params.addItem<SIManager>(p->siManager);

	//	d->softwareDetection = new JSoftwareDetection(p->account, p->params);

	foreach (const ObjectGenerator *gen, ObjectGenerator::module<JabberExtension>()) {
		if (JabberExtension *ext = gen->generate<JabberExtension>()) {
			d->extensions.append(ext);
			ext->init(this);
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
	Jreen::JID jid = unitId;
	if (!!(unit = d->conferenceManager.data()->muc(jid)))
		return unit;
	return d->roster->contact(jid, create);
	return 0;
}

void JAccount::loadSettings()
{
	Q_D(JAccount);
	Config cfg = config();
	cfg.beginGroup("general");

	d->priority = cfg.value("priority", 15);
	d->nick = cfg.value("nick", id());
	if (cfg.hasChildKey("photoHash"))
		setAvatarHex(cfg.value("photoHash", QString()));

	Jreen::JID jid(id());
	if (!d->client->isConnected()) {
		jid.setResource(cfg.value("resource", QLatin1String("qutIM/Jreen")));
	}
	d->client->setFeatureConfig(Client::Encryption, cfg.value("encryption", Client::Auto));
	d->client->setFeatureConfig(Client::Compression, cfg.value("compression", Client::Auto));
	{
		cfg.beginGroup("bosh");
		if (cfg.value("use", false)) {
			QString host = cfg.value("host", jid.domain());
			int port = cfg.value("port", 5280);
			d->client->setConnection(new ConnectionBOSH(host, port));
		}
		cfg.endGroup();
	}
	d->client->setJID(jid);
	d->client->setPassword(cfg.value("passwd", QString(), Config::Crypted));
	if(!cfg.value("autoDetect",true)) {
		d->client->setPort(cfg.value("port", 5222));
		d->client->setServer(cfg.value("server",d->client->server()));
	}

	cfg.endGroup();
	
	emit parametersChanged(d->parameters);
}

void JAccount::setPasswd(const QString &passwd)
{
	Q_D(JAccount);
	config().group("general").setValue("passwd",passwd, Config::Crypted);
	d->client->setPassword(passwd);
}

QStringList JAccount::updateParameters(const QVariantMap &parameters, bool forced)
{
	Q_D(JAccount);
	QStringList result;
	if (!forced && d->parameters == parameters)
		return result;
	
	if (forced)
		d->hasChangedParameters = false;
	
	QString password = parameters.value(QLatin1String("password")).toString();
	QString server = parameters.value(QLatin1String("connect-server")).toString();
	int port = parameters.value(QLatin1String("port"), -1).toInt();
	QString boshServer = parameters.value(QLatin1String("bosh-server")).toString();
	int boshPort = parameters.value(QLatin1String("bosh-port"), -1).toInt();
	QString resource = parameters.value(QLatin1String("resource")).toString();
	int priority = parameters.value(QLatin1String("priority"), 30).toInt();
	
	Presence &presence = d->client->presence();
	presence.setPriority(priority);
	if (d->client->isConnected()) {
		d->hasChangedParameters = true;
		d->client->send(presence);
		if (d->client->server() != server)
			result << QLatin1String("connect-server");
		if (d->client->port() != port)
			result << QLatin1String("port");
		if (d->client->jid().resource() != resource)
			result << QLatin1String("resource");
	} else {
		JID jid = d->client->jid();
		jid.setResource(resource);
		d->client->setServer(port < 0 ? jid.domain() : server);
		d->client->setPort(port);
	}
	d->client->setPassword(password);
	
	if (!forced) {
		Config config = Account::config();
		config.setValue(QLatin1String("resource"), resource);
		config.setValue(QLatin1String("priority"), priority);
		config.setValue(QLatin1String("autoDetect"), port < 0);
		config.setValue(QLatin1String("server"), server);
		config.setValue(QLatin1String("port"), port);
		config.setValue(QLatin1String("passwd"), password, Config::Crypted);
		config.beginGroup(QLatin1String("bosh"));
		config.setValue(QLatin1String("use"), !boshServer.isEmpty());
		config.setValue(QLatin1String("server"), boshServer);
		config.setValue(QLatin1String("port"), boshPort);
		config.endGroup();
	}
	
	d->parameters = parameters;
	emit parametersChanged(d->parameters);
	return result;
}

void JAccount::loadParameters()
{
	Q_D(JAccount);
	Config config = Account::config();
	d->parameters.clear();
	d->parameters.insert(QLatin1String("resource"), config.value(QLatin1String("resource"), "qutIM"));
	d->parameters.insert(QLatin1String("priority"), config.value(QLatin1String("priority")));
	if (config.value(QLatin1String("autoDetect"), -1) > 0) {
		d->parameters.insert(QLatin1String("connect-server"), config.value(QLatin1String("server")));
		d->parameters.insert(QLatin1String("port"), config.value(QLatin1String("port")));
	}
	d->parameters.insert(QLatin1String("password"), config.value(QLatin1String("passwd"), Config::Crypted));
	config.beginGroup(QLatin1String("bosh"));
	if (config.value(QLatin1String("use"), false)) {
		d->parameters.insert(QLatin1String("bosh-server"), config.value(QLatin1String("server")));
		d->parameters.insert(QLatin1String("bosh-port"), config.value(QLatin1String("port")));
	}
	updateParameters(d->parameters, true);
	emit parametersChanged(d->parameters);
}

void JAccount::virtual_hook(int id, void *data)
{
	switch (id) {
	case ReadParametersHook: {
		QVariantMap &parameters = *reinterpret_cast<QVariantMap*>(data);
		parameters = d_func()->parameters;
		break;
	}
	case UpdateParametersHook: {
		UpdateParametersArgument &argument = *reinterpret_cast<UpdateParametersArgument*>(data);
		argument.reconnectionRequired = updateParameters(argument.parameters);
		break;
	}
	default:
		Account::virtual_hook(id, data);
	}
}

JRoster *JAccount::roster() const
{
	return d_func()->roster;
}

QString JAccount::name() const
{
	return d_func()->nick;
}

QString JAccount::getPassword() const
{
	return d_func()->client->password();
}

JMessageSessionManager *JAccount::messageSessionManager() const
{
	return d_func()->messageSessionManager;
}

Jreen::Client *JAccount::client() const
{
	return d_func()->client.data();
}

JSoftwareDetection *JAccount::softwareDetection() const
{
	return d_func()->softwareDetection;
}

JMUCManager *JAccount::conferenceManager()
{
	return d_func()->conferenceManager.data();
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
		QNetworkProxy proxy = NetworkProxyManager::toNetworkProxy(NetworkProxyManager::settings(this));
		d->client->setProxy(proxy);
		if (d->passwordDialog) {
			/* nothing */
		} else if(d->client->password().isEmpty()) {
			d->passwordDialog = PasswordDialog::request(this);
			d->passwordDialog.data()->setProperty("status", qVariantFromValue(status));
			JPasswordValidator *validator = new JPasswordValidator(d->passwordDialog.data());
			connect(d->passwordDialog.data(), SIGNAL(finished(int)), SLOT(_q_on_password_finished(int)));
			d->passwordDialog.data()->setValidator(validator);
		} else {
			d->client->connectToServer();
			d->status = status;
			setAccountStatus(Status::instance(Status::Connecting, "jabber"));
		}
	} else if(status.type() == Status::Offline) {
		bool force = old.type() == Status::Connecting;
		if (force)
			setAccountStatus(Status::instance(Status::Offline, "jabber"));
		d->client->disconnectFromServer(true);
	} else if(old.type() != Status::Offline && old.type() != Status::Connecting) {
		d->applyStatus(status);
		//		d->client->setPresence(JStatus::statusToPresence(status), status.text());
	}
}

void JAccount::setAccountStatus(Status status)
{
	Q_D(JAccount);
	if (status != Status::Connecting && status != Status::Offline)
		d->conferenceManager.data()->setPresenceToRooms(d->client->presence());
	Account::setStatus(status);
}

QString JAccount::getAvatarPath()
{
	return QString("%1/avatars/%2")
			.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
			.arg(protocol()->id());
}

void JAccount::setAvatarHex(const QString &hex)
{
	Q_D(JAccount);
	Jreen::VCardUpdate::Ptr update = d->client->presence().payload<Jreen::VCardUpdate>();
	update->setPhotoHash(hex);
	if (!hex.isEmpty())
		d->avatar = getAvatarPath() + QLatin1Char('/') + hex;
	else
		d->avatar.clear();
	emit avatarChanged(d->avatar);
}

QString JAccount::avatar()
{
	return d_func()->avatar;
}

bool JAccount::event(QEvent *ev)
{
	return Account::event(ev);
}

QSet<QString> JAccount::features() const
{
	return d_func()->client->serverFeatures();
}

bool JAccount::checkFeature(const QString &feature) const
{
	return d_func()->client->serverFeatures().contains(feature);
}

bool JAccount::checkIdentity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	const Jreen::Disco::IdentityList identities = d->client->serverIdentities();
	bool ok = false;
	for (int i = 0; !ok && i < identities.size(); i++) {
		const Jreen::Disco::Identity &identity = identities[i];
		ok |= (identity.category() == category && identity.type() == type);
	}
	return ok;
}

QString JAccount::identity(const QString &category, const QString &type) const
{
	Q_D(const JAccount);
	const Jreen::Disco::IdentityList identities = d->client->serverIdentities();
	for (int i = 0; i < identities.size(); i++) {
		const Jreen::Disco::Identity &identity = identities[i];
		if (identity.category() == category && identity.type() == type)
			return identity.name();
	}
	return QString();
}

} // Jabber namespace

#include "jaccount.moc"

