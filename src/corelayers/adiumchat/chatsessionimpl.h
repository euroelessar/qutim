#ifndef CHATSESSIONIMPL_H
#define CHATSESSIONIMPL_H
#include <libqutim/messagesession.h>
#include "chatlayerimpl.h"

namespace AdiumChat
{
	class ChatSessionImpl : public ChatSession
	{
		Q_OBJECT
	public:
		virtual void addContact ( Contact* c );
		virtual void appendMessage ( const qutim_sdk_0_3::Message& message );
		virtual void chatStateChanged ( Contact* c, ChatState state );
		virtual void removeContact ( Contact* c );
	};
}
#endif // CHATSESSIONIMPL_H
