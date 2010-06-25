/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef JSESSIONCONVERTOR_H
#define JSESSIONCONVERTOR_H

#include <QObject>
#include "../servicediscovery/jservicediscovery.h"
#include "../servicediscovery/jservicereceiver.h"
#include "../roster/jmessagesession.h"
#include <gloox/mucroomhandler.h>

using namespace gloox;

namespace Jabber
{
	class JSessionConvertorPrivate;
	
	class JSessionConvertor : public QObject, public JServiceReceiver, public MUCRoomHandler
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JServiceReceiver)
		Q_DECLARE_PRIVATE(JSessionConvertor)
	public:
		explicit JSessionConvertor(JMessageSession *session);
		~JSessionConvertor();
		virtual void setInfo(int id);
		virtual void setItems(int id, const QList<JDiscoItem> &items);
		virtual void setError(int id);
		
		void handleMUCParticipantPresence(MUCRoom *room, const MUCRoomParticipant participant,
										  const Presence &presence);
		void handleMUCMessage(MUCRoom *room, const gloox::Message &msg, bool priv);
		bool handleMUCRoomCreation(MUCRoom *room);
		void handleMUCSubject(MUCRoom *room, const std::string &nick, const std::string &subject);
		void handleMUCInviteDecline(MUCRoom *room, const JID &invitee, const std::string &reason);
		void handleMUCError(MUCRoom *room, StanzaError error);
		void handleMUCInfo(MUCRoom *room, int features, const std::string &name,
						   const DataForm *infoForm);
		void handleMUCItems(MUCRoom *room, const Disco::ItemList &items);
	protected:
		void processServer(const QString &server);
	private:
		QScopedPointer<JSessionConvertorPrivate> d_ptr;
	};
}

#endif // JSESSIONCONVERTOR_H
