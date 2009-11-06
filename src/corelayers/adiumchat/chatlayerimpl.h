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
		virtual ~ChatLayerImpl();
	private slots:
		void onAccountDestroyed(QObject *object);
		void onSessionRemoved(Account *acc, const QString &id);
		void onChatWidgetDestroyed(QObject *object);
	private:
		inline QString getWidgetId (Account* acc, const QString& id) const;
		//QHash<Account * , QPair<QPointer<Account>, ChatSessionHash> >  m_chat_sessions;//TODO more safe
		QHash<Account * , ChatSessionHash >  m_chat_sessions;
		QHash<QString, ChatWidget *> m_chatwidget_list;
	};

}
#endif // CHATLAYERIMPL_H
