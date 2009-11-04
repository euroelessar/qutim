#ifndef FINDEMAIL_H
#define FINDEMAIL_H
#include <corelayers/adiumchat/messagemodifier.h> //TODO
#include "../chatsessionimpl.h"

namespace AdiumChat
{

class FindEmail : public MessageModifier
{
public:
	virtual QString getValue(const qutim_sdk_0_3::ChatSession* session, const qutim_sdk_0_3::Message& message, const QString& name, const QString& value);	
    virtual QStringList supportedNames() const;
};

}
#endif // FINDEMAIL_H
