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

namespace Jabber {

	class JPasswordValidator : public QValidator
	{
		State validate(QString &input, int &pos) const
		{
			if (input.isEmpty())
				return Intermediate;
			else
				return Acceptable;
		}
	};

	JAccount::JAccount(const QString &jid) : Account(jid, JProtocol::instance()), p(new JAccountPrivate)
	{
		loadSettings();
		Account::setStatus(Status::instance(Status::Offline, "jabber"));
		p->discoManager = 0;
		p->connection = new JConnection(this);
		p->connectionListener = new JConnectionListener(this);
		Q_UNUSED(new JServerDiscoInfo(this));
		p->roster = new JRoster(this);
		p->messageHandler = new JMessageHandler(this);
		p->conferenceManager = new JMUCManager(this);
		connect(p->conferenceManager, SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)), 
				SIGNAL(conferenceCreated(qutim_sdk_0_3::Conference*)));
		p->connection->initExtensions();
		autoconnect();
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
		ChatUnit *unit = 0;
		if (!!(unit = p->conferenceManager->muc(unitId)))
			return unit;
		return p->roster->contact(unitId, create);
	}

	void JAccount::beginChangeStatus(Presence::PresenceType presence)
	{
		p->connection->setConnectionPresence(presence);				
		Status previous = status();
		if (previous.type() == Status::Offline && presence != Presence::Unavailable) {
			Status newStatus = previous;
			newStatus.setType(Status::Connecting);
			Account::setStatus(newStatus);
			emit statusChanged(newStatus, previous);
		}
	}

	void JAccount::endChangeStatus(Presence::PresenceType presence)
	{
		Status previous = status();
		Status newStatus = JProtocol::presenceToStatus(presence);
		debug() << "status changed from" << int(previous.type()) << "to" << newStatus << newStatus.text();
		if (previous == Status::Connecting && newStatus != Status::Offline) {
			p->conferenceManager->syncBookmarks();
			resetGroupChatManager(p->conferenceManager->bookmarkManager());
		}
		if (previous != Status::Offline && newStatus == Status::Offline) {
			p->roster->setOffline();
			resetGroupChatManager();
		}
		p->conferenceManager->setPresenceToRooms(presence);
		Account::setStatus(newStatus);
		emit statusChanged(newStatus, previous);
	}

	void JAccount::autoconnect()
	{
		if (p->autoConnect) {
			if (p->keepStatus)
				beginChangeStatus(p->status);
			else
				beginChangeStatus(Presence::Available);
		}
	}

	void JAccount::loadSettings()
	{
		Config general = config();
		general.beginGroup("general");
		p->passwd = general.value("passwd", QString(), Config::Crypted);
		p->autoConnect = general.value("autoconnect", false);
		p->keepStatus = general.value("keepstatus", true);
		p->nick = general.value("nick", id());
		p->status = static_cast<Presence::PresenceType>(general.value("prevstatus", 8));
		general.endGroup();
	}

	void JAccount::setPasswd(const QString &passwd)
	{
		config().group("general").setValue("passwd",passwd, Config::Crypted);
		p->passwd = passwd;
		config().sync();
	}

	JServiceDiscovery *JAccount::discoManager()
	{
		if (!p->discoManager)
			p->discoManager = new JServiceDiscovery(this);
		return p->discoManager;
	}

	QString JAccount::name() const
	{
		return p->nick;
	}

	void JAccount::setNick(const QString &nick)
	{
		Config general = config("general");
		general.setValue("nick", nick);
		general.sync();
		QString previous = p->nick;
		p->nick = nick;
		emit nameChanged(nick, previous);
	}

	const QString &JAccount::password(bool *ok)
	{
		if (ok)
			*ok = true;
		if (p->passwd.isEmpty()) {
			if (ok)
				*ok = false;
			PasswordDialog *dialog = PasswordDialog::request(this);
			JPasswordValidator *validator = new JPasswordValidator();
			dialog->setValidator(validator);
			if (dialog->exec() == PasswordDialog::Accepted) {
				if (ok)
					*ok = true;
				p->passwd = dialog->password();
				if (dialog->remember()) {
					config().group("general").setValue("passwd", p->passwd, Config::Crypted);
					config().sync();
				}
			}
			delete validator;
			delete dialog;
		}
		return p->passwd;
	}

	JConnection *JAccount::connection()
	{
		return p->connection;
	}

	JMessageHandler *JAccount::messageHandler()
	{
		return p->messageHandler;
	}

	gloox::Client *JAccount::client()
	{
		return p->connection->client();
	}

	JMUCManager *JAccount::conferenceManager()
	{
		return p->conferenceManager;
	}

	void JAccount::setStatus(Status status)
	{
		beginChangeStatus(JProtocol::statusToPresence(status));
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
		return p->features;
	}

	bool JAccount::checkFeature(const QString &feature) const
	{
		return p->features.contains(feature);
	}

	bool JAccount::checkFeature(const std::string &feature) const
	{
		return p->features.contains(QString::fromStdString(feature));
	}

	bool JAccount::checkIdentity(const QString &category, const QString &type) const
	{
		Identities::iterator catItr = p->identities.find(category);
		return catItr == p->identities.end() ? false : catItr->contains(type);
	}

	bool JAccount::checkIdentity(const std::string &category, const std::string &type) const
	{
		return checkIdentity(QString::fromStdString(category), QString::fromStdString(type));
	}

	QString JAccount::identity(const QString &category, const QString &type) const
	{
		Identities::iterator catItr = p->identities.find(category);
		return catItr == p->identities.end() ? QString() : catItr->value(type);
	}

	std::string JAccount::identity(const std::string &category, const std::string &type) const
	{
		return identity(QString::fromStdString(category), QString::fromStdString(type)).toStdString();
	}

} // Jabber namespace
