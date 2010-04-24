/****************************************************************************
 *  jmucsession.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef JMUCSESSION_H
#define JMUCSESSION_H

#include <QByteArray>
#include <gloox/mucroom.h>
#include <gloox/jid.h>

Q_CORE_EXPORT uint qHash(const QByteArray &key);
inline uint qHash(const std::string &str)
{ return qHash(QByteArray::fromRawData(str.data(), str.size())); }

#include <QHash>
#include <qutim/conference.h>

namespace Jabber
{
	class JMessageSession;
	class JAccount;
	class JMUCUser;
	struct JMUCSessionPrivate;

	class JMUCSession :
			public qutim_sdk_0_3::Conference,
			public gloox::MUCRoomHandler,
			public gloox::MUCRoomConfigHandler
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(JMUCSession)
		public:
			JMUCSession(const gloox::JID &room, const QString &password, JAccount *account);
			JMUCSession(JMessageSession *session);
			~JMUCSession();
			QString id() const;
			void sendMessage(const qutim_sdk_0_3::Message &message);
			void setBookmarkIndex(int index);
			int bookmarkIndex();
			bool enabledConfiguring();
			bool isJoined();
			qutim_sdk_0_3::Buddy *me() const;
			ChatUnit *participant(const QString &nick);
			QString title() const;
		protected:
			void loadSettings();
			// MUCRoomHandler
			void handleMUCParticipantPresence(gloox::MUCRoom *room, const gloox::MUCRoomParticipant participant,
											  const gloox::Presence &presence);
			void handleMUCMessage(gloox::MUCRoom *room, const gloox::Message &msg, bool priv);
			bool handleMUCRoomCreation(gloox::MUCRoom *room);
			void handleMUCSubject(gloox::MUCRoom *room, const std::string &nick, const std::string &subject);
			void handleMUCInviteDecline(gloox::MUCRoom *room, const gloox::JID &invitee,
										const std::string &reason);
			void handleMUCError(gloox::MUCRoom *room, gloox::StanzaError error);
			void handleMUCInfo(gloox::MUCRoom *room, int features, const std::string &name,
								const gloox::DataForm *infoForm);
			void handleMUCItems(gloox::MUCRoom *room, const gloox::Disco::ItemList &items);
			// MUCRoomConfigHandler
			void handleMUCConfigList(gloox::MUCRoom *room, const gloox::MUCListItemList &items,
									 gloox::MUCOperation operation);
			void handleMUCConfigForm(gloox::MUCRoom *room, const gloox::DataForm &form);
			void handleMUCConfigResult(gloox::MUCRoom *room, bool success, gloox::MUCOperation operation);
			void handleMUCRequest(gloox::MUCRoom *room, const gloox::DataForm &form);
		public slots:
			void join();
			void leave();
			void showConfigDialog();
		private slots:
			void closeConfigDialog();
		signals:
			void initClose();
		private:
			QScopedPointer<JMUCSessionPrivate> d_ptr;
	};
}

#endif // JMUCSESSION_H
