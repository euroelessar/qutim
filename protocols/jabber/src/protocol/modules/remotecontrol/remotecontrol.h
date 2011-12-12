/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H


#include <QObject>
#include "../../../sdk/jabber.h"
#include <QHash>

//namespace Jabber
//{
//class JContactResource;
//class JAccount;

//class RemoteControl : public QObject, public JabberExtension, public gloox::AdhocCommandProvider
//{
//	Q_OBJECT
//	Q_INTERFACES(Jabber::JabberExtension)
//public:
//	RemoteControl();
//	void init(qutim_sdk_0_3::Account *account);
//	void handleAdhocCommand(const gloox::JID &from, const gloox::Adhoc::Command &command, const std::string &sessionID);
//	bool handleAdhocAccessRequest(const gloox::JID &from, const std::string &command);
//private:
//	JAccount *m_account;
//	gloox::Client *m_client;
//	gloox::Adhoc *m_adhoc;
//	gloox::JID m_myJid;
//};
//}

#endif // REMOTECONTROL_H

