#ifndef ADIUMCHATFORM_H
#define ADIUMCHATFORM_H

#include <QHash>
#include <QPointer>
#include <QWidget>

namespace qutim_sdk_0_3 {
	class ChatSession;
}

namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;

		class AdiumChatWidget;
		class ChatSessionImpl;
		typedef QHash<QString, QPointer<AdiumChatWidget> > ChatWidgetHash;
		class AdiumChatForm : public QObject
		{
			Q_OBJECT
			Q_CLASSINFO("Service", "ChatForm")
			Q_CLASSINFO("Uses", "ChatLayer")
			Q_CLASSINFO("Uses", "IconLoader")
		public:
			explicit AdiumChatForm();
			~AdiumChatForm();
			Q_INVOKABLE QWidgetList chatWidgets();
		private slots:
			void onSessionCreated(qutim_sdk_0_3::ChatSession*);
			void onChatWidgetDestroyed(QObject *object);
			void onSessionActivated(bool active);
		private:
			ChatWidgetHash m_chatwidgets;
			inline QString getWidgetId(ChatSessionImpl *sess) const;
			AdiumChatWidget *findWidget(ChatSession *sess) const;
		};

	}
}

#endif //ADIUMCHATFORM_H
