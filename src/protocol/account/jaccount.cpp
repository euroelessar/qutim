#include "jaccount.h"
#include "roster/jroster.h"
#include "roster/jmessagehandler.h"
#include "../jprotocol.h"

namespace Jabber {

	struct JAccountPrivate
	{
		inline JAccountPrivate() : keepStatus(false), autoConnect(false) {}
		inline ~JAccountPrivate() {}
		JConnection *connection;
		JRoster *roster;
		JConnectionListener *connectionListener;
		JMessageHandler *messageHandler;
		QString jid;
		QString passwd;
		bool keepStatus;
		bool autoConnect;
		Presence::PresenceType status;
	};

	JAccount::JAccount(const QString &jid) : Account(jid, JProtocol::instance()), p(new JAccountPrivate)
	{
		p->jid = jid;

		p->connection = new JConnection(this);
		p->connectionListener = new JConnectionListener(this);
		p->roster = new JRoster(this);
		p->messageHandler = new JMessageHandler(this);
		p->connection->initExtensions();
		loadSettings();
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
		if (ChatUnit *unit = p->messageHandler->getSession(unitId))
			return unit;
		return p->roster->contact(unitId, create);
	}

	void JAccount::beginChangeStatus(Presence::PresenceType presence)
	{
		p->connection->setConnectionPresence(presence);
	}

	void JAccount::endChangeStatus(Presence::PresenceType presence)
	{
		Account::setStatus(JProtocol::presenceToStatus(presence));
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
		p->passwd = config().group("general").value("passwd", QString(), Config::Crypted);
		p->autoConnect = config().group("general").value("autoconnect", false);
		p->keepStatus = config().group("general").value("keepstatus", true);
		p->status = static_cast<Presence::PresenceType>(
		config().group("general").value("prevstatus", 8));
	}

	const QString &JAccount::jid()
	{
		return p->jid;
	}

	const QString &JAccount::password(bool *ok)
	{
		if (ok)
			*ok = true;
		if (p->passwd.isEmpty()) {
			JInputPassword *inputPasswd = new JInputPassword(jid());
			if (inputPasswd->exec()) {
				p->passwd = inputPasswd->passwd();
				if (inputPasswd->isSave()) {
					config().group("general").setValue("passwd",
							p->passwd,
							Config::Crypted);
					config().sync();
				}
			} else {
				if (ok)
					*ok = false;
			}
			inputPasswd->close();
			delete inputPasswd;
		}
		return p->passwd;
	}

	JConnection *JAccount::connection()
	{
		return p->connection;
	}

	gloox::Client *JAccount::client()
	{
		return p->connection->client();
	}
} // Jabber namespace

