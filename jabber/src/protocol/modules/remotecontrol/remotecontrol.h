#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H


#include <QObject>
#include "sdk/jabber.h"
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
//	void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
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
