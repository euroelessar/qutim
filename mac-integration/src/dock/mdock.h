#ifndef MDOCK_H
#define MDOCK_H

#include <qutim/messagesession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>

namespace MacIntegration
{
	using namespace qutim_sdk_0_3;

	struct MDockPrivate;

	class MDock : public MenuController
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "DockManager")
		Q_CLASSINFO("Uses", "ContactList")
		Q_CLASSINFO("Uses", "ChatLayer")
		Q_CLASSINFO("Uses", "IconLoader")
		Q_DECLARE_PRIVATE(MDock)
		public:
			MDock();
			~MDock();
			void dockIconClickEvent();
		private slots:
			void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
			void onSessionDestroyed();
			void onUnreadChanged(const qutim_sdk_0_3::MessageList &unread);
			void onAccountCreated(qutim_sdk_0_3::Account *account);
			void setStatusIcon();
			void onStatusChanged();
			void onActivatedSession(bool state);
		protected:
			void createStatusAction(Status::Type type);
			void setBadgeLabel(const QString &string);
		private:
			QScopedPointer<MDockPrivate> d_ptr;
	};
}

#endif // MDOCK_H
