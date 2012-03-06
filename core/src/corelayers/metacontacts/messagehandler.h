#ifndef METACONTACTMESSAGEHANDLER_H
#define METACONTACTMESSAGEHANDLER_H

#include <qutim/messagehandler.h>

namespace Core
{
namespace MetaContacts
{
class MetaContactMessageHandler : public qutim_sdk_0_3::MessageHandler
{
public:
    MetaContactMessageHandler();
protected:
    virtual qutim_sdk_0_3::MessageHandler::Result doHandle(qutim_sdk_0_3::Message &message, QString *reason);
};

}
}

#endif // METACONTACTMESSAGEHANDLER_H
