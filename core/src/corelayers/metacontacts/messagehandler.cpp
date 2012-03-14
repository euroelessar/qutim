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
    if (message.isIncoming())
    {
        MetaContactImpl *contact = qobject_cast<MetaContactImpl*>(message.chatUnit()->metaContact());
        if(contact)
        {
            debug() << "Metacontact: " << contact;
            Contact* active = contact->getActiveContact();
            QList<Contact*> contacts = contact->contacts();
            debug() << "Active contact: " << active;
            if(active->buddy() != message.chatUnit()->buddy())
            {
                for (int i = 0; i < contacts.size(); i++)
                {
                    if (contacts.at(i)->account() == message.chatUnit()->account())
                    {
                        contact->setActiveContact(contacts.at(i));
                        return MetaContactMessageHandler::Accept;
                    }
                }
            }
        }
        debug() << message.chatUnit();
        debug() << message.chatUnit()->buddy();
        debug() << message.chatUnit()->account();
        debug() << message.chatUnit()->account()->protocol();
        debug() << message.text();
    }



    return MetaContactMessageHandler::Accept;

}

}
}
