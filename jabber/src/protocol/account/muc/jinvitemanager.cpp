#include "jinvitemanager.h"
#include "../jaccount.h"
#include "jmucmanager.h"
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
			const std::string &, const std::string &password, bool, const std::string &)
	{
		QString conferenceName(QString::fromStdString(room.full()));
		QString inviterName(QString::fromStdString(from.bare()));
		QString reasonMessage(QString::fromStdString(reason));
		QString passwordRoom(QString::fromStdString(password));
		QString message = tr("User %1 invite you\nto conference %2").arg(inviterName).arg(conferenceName);
		if (!reasonMessage.isEmpty())
			message = message % tr("\nwith reason \"%1\"").arg(reasonMessage);
		message = message % tr("\nAccept invitation?");
		if (QMessageBox(QMessageBox::Question, tr("Invite to groupchat"), message, QMessageBox::Yes | QMessageBox::No).exec()
				== QMessageBox::Yes)
			p->account->conferenceManager()->openJoinWindow(conferenceName, p->account->nick(), passwordRoom);
	}
}
