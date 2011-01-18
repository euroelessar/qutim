#include "astralaccount.h"
#include "astralprotocol.h"
#include "astralroster.h"
#include "astralsessionmanager.h"
#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingChannel>
#include <QDebug>

struct AstralAccountPrivate
{
	AccountPtr acc;
	ConnectionPtr conn;
	AstralProtocol *protocol;
	QPointer<AstralRoster> roster;
	AstralSessionManager *sessionManager;
	QHash<QString, QPointer<AstralSession> > sessions;
};

AstralAccount::AstralAccount(AstralProtocol *protocol, const QString &id) : qutim_sdk_0_3::Account(id, protocol), p(new AstralAccountPrivate)
{
	p->protocol = protocol;
//	p->roster = new AstralRoster(this, 0);
	ConnectionManager *connMgr = protocol->connectionManager().data();
	QVariantMap params;
	params["account"] = id;
	params["password"] = config("general").value("passwd", QString(), Config::Crypted);
//	if(id.contains("qutim.org"))
//		params["server"] = "jabber.qutim.org";
	qDebug() << params;

	QDBusMessage reply = connMgr->interface<Client::ConnectionManagerInterface>()->call("RequestConnection", protocol->id(), params);
	p->conn = Connection::create(reply.arguments().value(0).toString(), reply.arguments().value(1).value<QDBusObjectPath>().path());
	connect(p->conn->requestConnect(),
			SIGNAL(finished(Tp::PendingOperation*)),
			SLOT(onConnect(Tp::PendingOperation*)));
	qDebug() << connMgr->interfaces();
	QDBusMessage reply2 = p->conn->interface<Client::ConnectionInterface>()->call("GetInterfaces");
	QStringList interfaces = reply2.arguments().value(0).toStringList();
	qDebug() << interfaces;
	if(interfaces.contains(TELEPATHY_INTERFACE_CONNECTION_INTERFACE_REQUESTS))
	{
		qDebug() << "connecting to Connection.Interface.NewChannels";
		connect(p->conn->requestsInterface(),
				SIGNAL(NewChannels(const Tp::ChannelDetailsList&)),
				SLOT(onNewChannels(const Tp::ChannelDetailsList&)));
	}

//	connect(p->conn.data(),
//			SIGNAL(invalidated(Tp::DBusProxy *, const QString &, const QString &)),
//			SLOT(onConnectionInvalidated(Tp::DBusProxy *, const QString &, const QString &)));
}

AstralAccount::~AstralAccount()
{
}

void AstralAccount::setStatus(Status status)
{
}

ChatUnit *AstralAccount::getUnitForSession(ChatUnit *unit)
{
	if(AstralContact *contact = qobject_cast<AstralContact *>(unit))
	{
		Q_UNUSED(contact);
//		Tp::Contact
//		TextChannel::create(p->conn, contact->ptr());
//
//		QVariantMap request;
//		request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
//					   TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT);
//		request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
//					   Tp::HandleTypeContact);
//		request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandle"),
//					   contact ? contact->handle().at(0) : 0);
//		return new PendingChannelRequest(dbusConnection(), objectPath(),
//				request, userActionTime, preferredHandler, false, this);
	}
	return unit;
}

ChatUnit *AstralAccount::getUnit(const QString &unitId, bool create)
{
	ChatUnit *unit = p->sessions.value(unitId);
	if(!unit && p->roster)
		unit = p->roster->contact(unitId);
	return unit;
}

AstralRoster *AstralAccount::roster()
{
	return p->roster;
}

AstralSessionManager *AstralAccount::sessionManager()
{
	return p->sessionManager;
}

void AstralAccount::onConnect(PendingOperation *op)
{
	if (op->isError())
	{
		qWarning() << "Connection cannot become connected";
		return;
	}

	PendingReady *pr = qobject_cast<PendingReady *>(op);
	ConnectionPtr conn = ConnectionPtr(qobject_cast<Connection *>(pr->object()));
	p->roster = new AstralRoster(this, conn);
}

void AstralAccount::onConnectError(PendingOperation *op)
{
//	qDebug() << "AstralAccount::onConnectError: connection became invalid:" <<
//		errorName << "-" << errorMessage;
}

void AstralAccount::onNewChannels(const Tp::ChannelDetailsList &channels)
{
	qDebug() << "AstralAccount::onNewChannels";
	foreach (const Tp::ChannelDetails &details, channels)
	{
		QString channelType = details.properties.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString();
		bool requested = details.properties.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Requested")).toBool();
		qDebug() << " channelType:" << channelType;
		qDebug() << " requested  :" << requested;
		qDebug() << " channelPath:" << details.channel.path();
		qDebug() << " properties :" << details.properties;

		if (channelType == TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT && !requested)
		{
			TextChannelPtr channel = TextChannel::create(p->conn,
														 details.channel.path(),
														 details.properties);
		}
	}
}

