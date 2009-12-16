#include "jconnection.h"
#include <QDebug>
#include <QTimer>

namespace Jabber
{
	struct JConnectionPrivate
	{
		JAccount *account;
		Client *client;
		QString resource;
		bool isAutoPriority;
		QMap<Presence::PresenceType, int> priority;
		QString password;
	};

	JConnection::JConnection(JAccount *account) : p(new JConnectionPrivate)
	{
		p->account = account;
		p->password = QString();
		loadSettings();
		JID jid = JID(account->jid().toStdString());
		jid.setResource(p->resource.toStdString());
		p->client = new Client(jid, p->password.toStdString());
		p->client->disco()->setVersion("qutIM", "0.3");
		p->client->disco()->setIdentity("client", "pc");
		p->client->registerPresenceHandler(this);
	}

	void JConnection::handlePresence(const Presence &presence)
	{
		qDebug() << presence.presence();
		qDebug() << p->client->state();
	}

	void JConnection::loadSettings()
	{
		QString defaultResource = JProtocol::instance()->config().group("general").value("defaultresource", QString("qutIM"));
		ConfigGroup group = p->account->config().group("general");
		/*bool lbookmark = group.value("localbookmark", false);
		if (lbookmark != p->isLocalBookmark)
		{
			p->isLocalBookmark = local_bookmark;
			requestBookmarks();
		}*/
		p->resource = group.value("resource", defaultResource);
		p->priority.clear();
		p->priority.insert(Presence::Invalid, group.value("proirity", 3));
		p->isAutoPriority = group.value("autopriority",true);
		if (p->isAutoPriority)
		{
			group = JProtocol::instance()->config().group("priority");
			p->priority.insert(Presence::Available, group.value("online", 3));
			p->priority.insert(Presence::Chat, group.value("online", 3));
			p->priority.insert(Presence::Away, group.value("away", 2));
			p->priority.insert(Presence::XA, group.value("na", 1));
			p->priority.insert(Presence::DND, group.value("dnd", -1));
		}
	}

	void JConnection::setConnectionPresence(Presence::PresenceType presence)
	{
		if (p->password.isEmpty()) {
			bool ok;
			p->password = p->account->password(&ok);
			if (ok)
				p->client->setPassword(p->password.toStdString());
			else
				return;
			qDebug() << p->password;
		}
		p->client->setPresence(presence, p->isAutoPriority
				? p->priority.value(presence) : p->priority.value(Presence::Invalid));
		if (p->client->state() == StateDisconnected) {
			p->client->connect(false);
			QTimer *timer = new QTimer();
			timer->setInterval(100);
			connect(timer, SIGNAL(timeout()), this, SLOT(checkData()));
			timer->start();
		}
		qDebug() << p->client->presence().presence() << "***************************";
	}

	void JConnection::checkData()
	{
		p->client->recv();
	}
}
