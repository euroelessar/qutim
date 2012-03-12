#include "messagehandler.h"
#include <qutim/debug.h>

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
    debug() << Q_FUNC_INFO;
    debug() << message.text();
    if (!message.isIncoming())
    {
        return MetaContactMessageHandler::Accept;
    }

    return MetaContactMessageHandler::Accept;

}

}
}
