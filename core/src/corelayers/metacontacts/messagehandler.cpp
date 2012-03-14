#include "messagehandler.h"
#include <qutim/debug.h>
#include <qutim/metacontact.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include "metacontactimpl.h"

using namespace qutim_sdk_0_3;

namespace Core
{
namespace MetaContacts
{
MetaContactMessageHandler::MetaContactMessageHandler()
{
}

MessageHandler::Result MetaContactMessageHandler::doHandle(qutim_sdk_0_3::Message &message, QString *reason)
{
	if (message.isIncoming()) {
		if (MetaContactImpl *contact = qobject_cast<MetaContactImpl*>(message.chatUnit()->metaContact())) {
			Contact *rawContact = 0;
			ChatUnit *u = message.chatUnit();
			while (u) {
			   if ((rawContact = qobject_cast<Contact*>(u)))
						break;
					u = u->upperUnit();
			}
			if (rawContact && contact->getActiveContact()->buddy() != rawContact)
				contact->setActiveContact(rawContact);
		}
	}
	return MetaContactMessageHandler::Accept;

}

}
}
