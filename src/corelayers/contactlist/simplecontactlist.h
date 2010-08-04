#ifndef SIMPLECONTACTLIST_H
#define SIMPLECONTACTLIST_H

#include "libqutim/actiontoolbar.h"
#include "libqutim/status.h"
#include "libqutim/contact.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace SimpleContactList
	{
		struct ModulePrivate;

		class Module : public MenuController
		{
			Q_OBJECT
			Q_PROPERTY(QWidget* widget READ widget)
			Q_CLASSINFO("Service", "ContactList")
			Q_CLASSINFO("Uses", "IconLoader")
			Q_CLASSINFO("Uses", "MetaContactManager")
		public:
			Module();
			virtual ~Module();			
			Q_INVOKABLE void addButton(ActionGenerator *generator);
			Q_INVOKABLE QWidget *widget();
		protected:
			bool event(QEvent *);
			bool eventFilter(QObject *, QEvent *);
		public slots:
			void show();
			void hide();
			void changeVisibility();
			void addContact(qutim_sdk_0_3::Contact *contact);
		private slots:
			void onConfigureClicked(QObject*);
			void onAccountCreated(qutim_sdk_0_3::Account *account);
			void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
			void onAccountDestroyed(QObject *obj);
			void onStatusChanged();
			void onSearchButtonToggled(bool toggled);
			void showStatusDialog();
			void changeStatusTextAccepted();
			void onCopyIdTriggered(QObject *obj);
			void onQuitTriggered(QObject*);
		private:
			QAction *createGlobalStatusAction(Status::Type type); //FIXME, temporary. Will be replaced by metacontact manager
			QScopedPointer<ModulePrivate> p;
		};
	}
}

#endif // SIMPLECONTACTLIST_H
