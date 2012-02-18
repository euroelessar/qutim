/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JSESSIONCONVERTOR_H
#define JSESSIONCONVERTOR_H

#include <QObject>
//#include <jreen/disco.h>
#include "../roster/jmessagesession.h"

//using namespace gloox;

//namespace Jabber
//{
//	class JSessionConvertorPrivate;
	
//	class JSessionConvertor : public QObject, public JServiceReceiver, public MUCRoomHandler
//	{
//		Q_OBJECT
//		Q_INTERFACES(Jabber::JServiceReceiver)
//		Q_DECLARE_PRIVATE(JSessionConvertor)
//	public:
//		explicit JSessionConvertor(JMessageSession *session);
//		~JSessionConvertor();
//		virtual void setInfo(int id);
//		virtual void setItems(int id, const QList<JDiscoItem> &items);
//		virtual void setError(int id);
		
//		void handleMUCParticipantPresence(MUCRoom *room, const MUCRoomParticipant participant,
//										  const Presence &presence);
//		void handleMUCMessage(MUCRoom *room, const gloox::Message &msg, bool priv);
//		bool handleMUCRoomCreation(MUCRoom *room);
//		void handleMUCSubject(MUCRoom *room, const std::string &nick, const std::string &subject);
//		void handleMUCInviteDecline(MUCRoom *room, const JID &invitee, const std::string &reason);
//		void handleMUCError(MUCRoom *room, StanzaError error);
//		void handleMUCInfo(MUCRoom *room, int features, const std::string &name,
//						   const DataForm *infoForm);
//		void handleMUCItems(MUCRoom *room, const Disco::ItemList &items);
//	protected:
//		void processServer(const QString &server);
//	private:
//		QScopedPointer<JSessionConvertorPrivate> d_ptr;
//	};
//}

#endif // JSESSIONCONVERTOR_H

