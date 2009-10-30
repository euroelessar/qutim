#ifndef CHATLAYERIMPL_H
#define CHATLAYERIMPL_H
#include <libqutim/messagesession.h>
#include <QPointer>

class ChatWidget;
using namespace qutim_sdk_0_3;

namespace AdiumChat
{

	typedef QHash<QString,  ChatSession *> ChatSessionHash;
	class ChatLayerImpl : public ChatLayer
	{
		Q_OBJECT
	public:
		virtual ChatSession* getSession ( Account* acc, const QString& id, bool create = true );
		virtual QList<ChatSession* > sessions();
		ChatLayerImpl();
	private:
		//QHash<Account * , QPair<QPointer<Account>, ChatSessionHash> >  m_chat_sessions;//TODO more safe
		QHash<Account * , ChatSessionHash >  m_chat_sessions;
		QList<ChatWidget *> m_chatwidget_list;
	};

}
#endif // CHATLAYERIMPL_H
