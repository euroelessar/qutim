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
//#include "jpgpsupport.h"

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

	QString invisible = QStringLiteral("invisible");
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
    // FIXME: Return GPG support
    {
        Presence::Type type = JStatus::statusToPresence(status);
        client->setPresence(type, status.text(), priority);
        const Presence presence = client->presence();
        // We need this for peps
        Presence copy(presence.subtype(), client->jid().bareJID(), presence.status(), presence.priority());
        client->send(copy);
        q->conferenceManager()->setPresenceToRooms(presence);
    }
//	JPGPSupport::instance()->send(q, status, priority);

	q->setState(Account::Connected);
}

void JAccountPrivate::setPresence(Jreen::Presence presence)
{
	Q_Q(JAccount);
	Status now = JStatus::presenceToStatus(presence.subtype());
	now.setText(presence.status());
	q->setEffectiveStatus(now);
	if(presence.subtype() == Jreen::Presence::Unavailable)
		client->disconnectFromServer(false);
}

void JAccountPrivate::_q_connected()
{
	Q_Q(JAccount);
//	if (currentPGPKeyId != pgpKeyId) {
//		currentPGPKeyId = pgpKeyId;
//		emit q->pgpKeyIdChanged(currentPGPKeyId);
//	}
	applyStatus(q->status());
	conferenceManager.data()->syncBookmarks();
	q->resetGroupChatManager(conferenceManager.data()->bookmarkManager());	
	client->setPingInterval(q->config().group("general").value("pingInterval", 30000));
}

void JAccountPrivate::onPasswordReceived(const QString &password)
{
	client->setPassword(password);
	client->connectToServer();
}

void JAccountPrivate::_q_on_module_loaded(int i)
{
	qDebug() << Q_FUNC_INFO << loadedModules << i << q_func()->sender();
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

	Status::ChangeReason statusReason = Status::ByUnknown;

	switch(reason) {
	case Client::User:
		break;
	case Client::AuthorizationError: {
		statusReason = Status::ByAuthorizationFailed;
		keyChain->remove(keyChainId());
		client->setPassword(QString());
		break;
	}
	case Client::HostUnknown:
	case Client::ItemNotFound:
	case Client::SystemShutdown:
	case Client::NoSupportedFeature:
	case Client::NoAuthorizationSupport:
	case Client::NoEncryptionSupport:
	case Client::NoCompressionSupport:
		statusReason = Status::ByFatalError;
		break;
	case Client::RemoteStreamError:
	case Client::RemoteConnectionFailed:
	case Client::InternalServerError:
	case Client::Conflict:
	case Client::Unknown:
		statusReason = Status::ByNetworkError;
		break;
	}

	q->resetGroupChatManager(0);
	loadedModules = 0;

	q->setState(Account::Disconnected, statusReason);
}

void JAccountPrivate::_q_init_extensions(const QSet<QString> &features)
{
	//Q_Q(JAccount);
	qDebug() << "received features list" << features;
	//	foreach (const ObjectGenerator *gen, ObjectGenerator::module<JabberExtension>()) {
	//		if (JabberExtension *ext = gen->generate<JabberExtension>()) {
	//			qDebug() << "init ext" << ext;
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

	d->loadedModules = 0;
	d->privacyManager = new PrivacyManager(d->client.data());
	d->pubSubManager = new Jreen::PubSub::Manager(d->client.data());
	d->privateXml = new Jreen::PrivateXml(d->client.data());
	d->roster = new JRoster(this);
	Jreen::Capabilities::Ptr caps = d->client->presence().payload<Jreen::Capabilities>();
	caps->setNode(QLatin1String("http://qutim.org/"));
	d->conferenceManager = new JMUCManager(this, this);
	d->messageSessionManager = new JMessageSessionManager(this);
	connect(d->messageSessionManager, SIGNAL(messageEcnrypted(quint64)), SIGNAL(messageEcnrypted(quint64)));
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
	
	d->roster->loadFromStorage();

	connect(d->conferenceManager.data(), SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)),
			this, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));

	foreach (const ObjectGenerator *gen, ObjectGenerator::module<JabberExtension>()) {
		if (JabberExtension *ext = gen->generate<JabberExtension>()) {
			d->extensions.append(ext);
			ext->init(this);
		}
	}

	{
		// Temporary hook
		Config config = this->config(QStringLiteral("general"));
		const QString passwd = QStringLiteral("passwd");
		if (config.hasChildKey(passwd)) {
			d->keyChain->write(d->keyChainId(), config.value(passwd, QString(), Config::Crypted));
			config.remove(passwd);
		}
	}
//	JPGPSupport::instance()->addAccount(this);
}

JAccount::~JAccount()
{
//	JPGPSupport::instance()->removeAccount(this);
}

ChatUnit *JAccount::getUnitForSession(ChatUnit *unit)
{
	Q_D(JAccount);
	if (JContactResource *resource = qobject_cast<JContactResource*>(unit)) {
		if (ChatUnit *self = d->roster->selfContact(resource->id()))
			unit = self;
		if (!qobject_cast<JMUCUser*>(unit))
			unit = resource->upperUnit();
	}
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
//	d->pgpKeyId = cfg.value("pgpKeyId", QString());

	Jreen::JID jid(id());
	if (!d->client->isConnected()) {
		jid.setResource(cfg.value("resource", QLatin1String("qutIM")));
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
	if(!cfg.value("autoDetect",true)) {
		d->client->setPort(cfg.value("port", 5222));
		d->client->setServer(cfg.value("server",d->client->server()));
	}

	cfg.endGroup();
}

void JAccount::setPasswd(const QString &passwd)
{
	Q_D(JAccount);
	d->keyChain->write(d->keyChainId(), passwd);
	d->client->setPassword(passwd);
}

//QString JAccount::pgpKeyId() const
//{
//	return d_func()->currentPGPKeyId;
//}

void JAccount::virtual_hook(int id, void *data)
{
	switch (id) {
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

void JAccount::doConnectToServer()
{
	Q_D(JAccount);

	QNetworkProxy proxy = NetworkProxyManager::toNetworkProxy(NetworkProxyManager::settings(this));
	d->client->setProxy(proxy);
	if (d->passwordDialog) {
		/* nothing */
	} else if(d->client->password().isEmpty()) {
		d->keyChain->read(d->keyChainId()).connect(this, [this, d] (const KeyChain::ReadResult &result) {
			if (result.error == KeyChain::NoError) {
				d->onPasswordReceived(result.textData);
			} else if (!d->passwordDialog) {
				d->passwordDialog = PasswordDialog::request(this);

				connect(d->passwordDialog.data(), &PasswordDialog::finished, this, [this, d] (int result) {
					Q_ASSERT(sender() == d->passwordDialog.data());
					d->passwordDialog->deleteLater();

					if (result != PasswordDialog::Accepted) {
						setState(Disconnected, Status::ByPasswordUnknown);
						return;
					}

					if (d->passwordDialog->remember()) {
						d->keyChain->write(d->keyChainId(), d->passwordDialog->password());
					}

					d->onPasswordReceived(d->passwordDialog->password());
				});
			}
		});
	} else {
		d->client->connectToServer();
	}
}

void JAccount::doDisconnectFromServer()
{
	Q_D(JAccount);

	d->client->disconnectFromServer(true);

	setState(Disconnected, Status::ByUser);
}

void JAccount::doStatusChange(const Status &status)
{
	Q_D(JAccount);

	d->applyStatus(status);
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

#include "moc_jaccount.cpp"

