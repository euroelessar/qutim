#ifndef CHATLAYERIMPL_H
#define CHATLAYERIMPL_H
#include <libqutim/messagesession.h>
#include <QPointer>

using namespace qutim_sdk_0_3;

namespace AdiumChat
{

	class ChatWidget;
	class ChatSessionImpl;
	typedef QHash<QString,  ChatSessionImpl *> ChatSessionHash;
	typedef QList<ChatSessionImpl *> ChatSessionList;
	class ChatLayerImpl : public ChatLayer
	{
		Q_OBJECT
	public:
		virtual ChatSession* getSession ( Account* acc, const QString& id, bool create = true );
		virtual QList<ChatSession* > sessions();
		ChatLayerImpl();
	private slots:
		void onAccountDestroyed(QObject *object);
		void onSessionRemoved(Account *acc, const QString &id);
	private:
		//QHash<Account * , QPair<QPointer<Account>, ChatSessionHash> >  m_chat_sessions;//TODO more safe
		QHash<Account * , ChatSessionHash >  m_chat_sessions;
		QList<ChatWidget *> m_chatwidget_list;
	};

}
#endif // CHATLAYERIMPL_H
