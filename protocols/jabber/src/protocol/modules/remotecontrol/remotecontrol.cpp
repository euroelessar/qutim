//#include "remotecontrol.h"
//#include <qutim/account.h>
//#include "../../account/jaccount.h"

//using namespace qutim_sdk_0_3;

//namespace Jabber
//{
//static const char* presenceTxts[] =
//{
//	"online", "chat", "away", "dnd", "xa", "offline"
//};

//RemoteControl::RemoteControl()
//{
//}

//void RemoteControl::init(Account *account)
//{
//	m_account = static_cast<JAccount *>(account);
//	m_myJid.setJID(m_account->id().toStdString());
//	m_client = params.item<Client>();
//	m_adhoc = params.item<Adhoc>();
//	m_adhoc->registerAdhocCommandProvider(this, "set-status", "Set status");
//}

//void RemoteControl::handleAdhocCommand(const JID &from, const Adhoc::Command &command, const std::string &sessionID)
//{
//	if (!handleAdhocAccessRequest(from, command.node())) {
//		// TODO: return access forbidden error
//		return;
//	}

//	if (command.node() == "set-status") {
//		if (command.action() == Adhoc::Command::Execute) {
//			const DataForm *constForm = command.form();
//			if (constForm) {
//				DataFormField *status = constForm->field("status");
//				if (!status || status->value().empty()) {
//					// Send error stanza
//					return;
//				}
//				Presence::PresenceType presence =
//						(Presence::PresenceType)util::deflookup(status->value(),
//																presenceTxts,
//																Presence::Invalid);
//				if (presence == Presence::Invalid) {
//					// Send error stanza
//					return;
//				}
//				m_client->presence().setPresence(presence);
//				if (DataFormField *field = constForm->field("status-priority"))
//					m_client->presence().setPriority(atoi(field->value().data()));
//				if (DataFormField *field = constForm->field("status-message")) {
//					std::string message;
//					foreach (const std::string &str, field->values())
//						message.append(str).append("\n");
//					if (message.size() > 0)
//						message.resize(message.size() - 1);
//					m_client->presence().resetStatus();
//					m_client->presence().addStatus(message);
//				}
//				m_client->setPresence();
//				m_adhoc->respond(from, new Adhoc::Command(command.node(), sessionID,
//														  Adhoc::Command::Completed));
//			} else {
//				StringList instructions;
//				instructions.push_back("Choose the status and status message");
//				DataForm *form = new DataForm(TypeForm, instructions, "Change Status");
//				Presence &pres = m_client->presence();
//				form->addField(DataFormField::TypeHidden, "FORM_TYPE", "http://jabber.org/protocol/rc");
//				{
//					DataFormField *field = new DataFormField(DataFormField::TypeListSingle);
//					field->setName("status");
//					field->setLabel("Status");
//					field->setRequired(true);
//					field->addOption("Chat", "chat");
//					field->addOption("Online", "online");
//					field->addOption("Away", "away");
//					field->addOption("Extended Away", "xa");
//					field->addOption("Do Not Disturb", "dnd");
//					field->addOption("Offline", "offline");
//					field->setValue(presenceTxts[pres.subtype()]);
//					form->addField(field);
//				}
//				form->addField(DataFormField::TypeTextSingle, "status-priority",
//							   util::int2string(pres.priority()), "Priority");
//				form->addField(DataFormField::TypeTextMulti, "status-message",
//							   pres.status(), "Message");
//				m_adhoc->respond(from, new Adhoc::Command(command.node(), sessionID,
//														  Adhoc::Command::Executing, form));
//			}
//		}
//	}
//}

//bool RemoteControl::handleAdhocAccessRequest(const JID &from, const std::string &command)
//{
//	return from.bare() == m_myJid.bare();
//}
//}
