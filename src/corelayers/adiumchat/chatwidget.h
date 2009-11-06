#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include "chatlayerimpl.h"

namespace Ui
{
	class AdiumChatForm;
}
namespace AdiumChat
{
	enum ChatFlag
	{
		RemoveSessionOnClose	=	0x1, //remove session, when widget or tab was closed
		AeroThemeIntegeation	=	0x2, 
	};
	class ChatSessionImpl;
	class ChatWidget : public QWidget
	{
		Q_OBJECT
	public:
		ChatWidget(ChatFlag chatFlag = RemoveSessionOnClose);
		void clear();//remove all sessions
		ChatSessionList getSessionList() const;
		virtual ~ChatWidget();
	public slots:
		void addSession(ChatSessionImpl *session);
		void addSession(const ChatSessionList &sessions);
		void removeSession(ChatSessionImpl *session);
		void raise(ChatSessionImpl *session);
	private:
		ChatSessionList m_sessions;
		Ui::AdiumChatForm *ui;
		ChatFlag m_chat_flags;
	private slots:
		void currentIndexChanged (int index);
		void onCloseRequested(int index);
		void onTabMoved(int from,int to);
		void onSessionRemoved();
		void onSendButtonClicked();
	};

}
#endif // CHATWIDGET_H
