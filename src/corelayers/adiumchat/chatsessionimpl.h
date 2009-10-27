#ifndef CHATSESSIONIMPL_H
#define CHATSESSIONIMPL_H
#include <libqutim/messagesession.h>
#include <QPointer>

class QWebPage;
using namespace qutim_sdk_0_3;
namespace AdiumChat
{
	class ChatStyleOutput;
	class ChatSessionImpl : public ChatSession
	{
		Q_OBJECT
	public:
		ChatSessionImpl ( ChatLayer* chat );
		virtual ~ChatSessionImpl();
		virtual void addContact ( Contact* c );
		virtual void appendMessage ( const Message& message );
		virtual void chatStateChanged ( Contact* c, ChatState state );
		virtual void removeContact ( Contact* c );
	private:
		QPointer<QWebPage> m_web_page;
		QScopedPointer<ChatStyleOutput> m_chat_style_output;
	};
}
#endif // CHATSESSIONIMPL_H
