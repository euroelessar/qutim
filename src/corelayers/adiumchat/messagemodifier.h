#ifndef ADIUMMESSAGEMODIFIER_H
#define MESSAGEMODIFIER_H

#include <QObject>
#include "chatsessionimpl.h"

namespace qutim_sdk_0_3
{
	class ChatSession;
	class Message;
}

namespace AdiumChat
{
	class MessageModifier
	{
	public:
		virtual QString getValue(const ChatSession *session, const Message &message, const QString &name, const QString &value) = 0;
		virtual QStringList supportedNames() const = 0;
	};
}

Q_DECLARE_INTERFACE(AdiumChat::MessageModifier, "org.qutim.AdiumChat.MessageModifier")

#endif // ADIUMMESSAGEMODIFIER_H
