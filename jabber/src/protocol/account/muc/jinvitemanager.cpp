#include "jinvitemanager.h"
#include "../jaccount.h"
#include "jmucmanager.h"
#include "jmucsession.h"
#include "../roster/jmessagehandler.h"
#include "../roster/jmessagesession.h"
#include <gloox/mucroom.h>
#include <QStringBuilder>
#include <QMessageBox>

namespace Jabber
{
	struct JInviteManagerPrivate
	{
		JAccount *account;
	};

	JInviteManager::JInviteManager(JAccount *account) : MUCInvitationHandler(account->client()), p(new JInviteManagerPrivate)
	{
		p->account = account;
		p->account->client()->registerMUCInvitationHandler(this);
	}

	JInviteManager::~JInviteManager()
	{

	}

	void JInviteManager::handleMUCInvitation(const JID &room, const JID &from, const std::string &reason,
			const std::string &, const std::string &password, bool cont, const std::string &thread)
	{
		if (cont) {
			QString threadId = QString::fromStdString(thread);
			JMessageSession *session = p->account->messageHandler()->getSession(threadId);
			if (session) {
				JID jid = room;
				jid.setResource(p->account->name().toStdString());
				MUCRoom *room = new MUCRoom(p->account->client(), jid, 0, 0);
				JMUCSession *mucSession = new JMUCSession(p->account, room, thread);
				ChatLayer::get(session->upperUnit(), true)->setChatUnit(mucSession);
				session->deleteLater();
				mucSession->join();
				return;
			}
		}
		
		QString inviterName(QString::fromStdString(from.bare()));
		if (inviterName.isEmpty())
			return;
		QString conferenceName(QString::fromStdString(room.full()));
		QString reasonMessage(QString::fromStdString(reason));
		QString passwordRoom(QString::fromStdString(password));
		QString message = tr("User %1 invite you\nto conference %2").arg(inviterName).arg(conferenceName);
		if (!reasonMessage.isEmpty())
			message = message % tr("\nwith reason \"%1\"").arg(reasonMessage);
		message = message % tr("\nAccept invitation?");

		QMessageBox box (QMessageBox::Question,
						tr("Invite to groupchat"),
						message,
						QMessageBox::Yes | QMessageBox::No
						);
		if (box.exec() == QMessageBox::Yes)
			p->account->conferenceManager()->join(conferenceName,p->account->name(),passwordRoom);
	}
}
