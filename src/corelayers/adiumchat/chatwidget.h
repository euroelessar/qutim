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
	class ChatSessionImpl;
	class ChatWidget : public QWidget
	{
		Q_OBJECT
	public:
		ChatWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
		void clear();//remove all sessions
	public slots:
		void addSession(ChatSessionImpl *session);
		void removeSession(ChatSessionImpl *session);
	private:
		ChatSessionList m_sessions;
		Ui::AdiumChatForm *ui;
	private slots:
		void currentIndexChanged (int);
		void onSessionRemoved(); 
	};

}
#endif // CHATWIDGET_H
