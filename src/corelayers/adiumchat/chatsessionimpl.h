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
		//additional info and flags
		bool m_history_loaded;
		QString m_previous_sender; //me or nme (not me) //FIXME need refactoring in future
		int m_message_count;
	};
}
#endif // CHATSESSIONIMPL_H
