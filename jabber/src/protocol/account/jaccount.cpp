#include "jaccount.h"
#include "roster/jroster.h"
#include "roster/jmessagehandler.h"
#include "servicediscovery/jservicebrowser.h"
#include "servicediscovery/jservicediscovery.h"
#include "../jprotocol.h"
#include "muc/jmucmanager.h"
#include <qutim/systeminfo.h>
#include <qutim/passworddialog.h>
#include <qutim/debug.h>

namespace Jabber {

	struct JAccountPrivate
	{
		inline JAccountPrivate() : keepStatus(false), autoConnect(false) {}
		inline ~JAccountPrivate() {}
		JConnection *connection;
		JRoster *roster;
		JConnectionListener *connectionListener;
		JMessageHandler *messageHandler;
		QString passwd;
		QString nick;
		bool keepStatus;
		bool autoConnect;
		Presence::PresenceType status;
		JMUCManager *conferenceManager;
		QPointer<JServiceDiscovery> discoManager;
	};

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
		p->roster = new JRoster(this);
		p->messageHandler = new JMessageHandler(this);
		p->conferenceManager = new JMUCManager(this);
		p->connection->initExtensions();
		autoconnect();
	}

	JAccount::~JAccount()
	{
	}

	ChatUnit *JAccount::getUnitForSession(ChatUnit *unit)
	{
		return p->messageHandler->getSession(unit);
	}

	ChatUnit *JAccount::getUnit(const QString &unitId, bool create)
	{
		ChatUnit *unit = 0;
		if (!!(unit = p->conferenceManager->muc(unitId)))
			return unit;
		if (!!(unit = p->messageHandler->getSession(unitId)))
			return unit;
		return p->roster->contact(unitId, create);
	}

	void JAccount::beginChangeStatus(Presence::PresenceType presence)
	{
		p->connection->setConnectionPresence(presence);
	}

	void JAccount::endChangeStatus(Presence::PresenceType presence)
	{
		Status newStatus = JProtocol::presenceToStatus(presence);
		debug() << "new status" << newStatus << newStatus.text();
		if (status() == Status::Offline && newStatus != Status::Offline)
			p->conferenceManager->syncBookmarks();
		if (status() != Status::Offline && newStatus == Status::Offline)
			p->roster->setOffline();
		p->conferenceManager->setPresenceToRooms(presence);
		Account::setStatus(newStatus);
		emit statusChanged(newStatus);
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
		p->nick = nick;
		emit nameChanged(nick);
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
} // Jabber namespace
