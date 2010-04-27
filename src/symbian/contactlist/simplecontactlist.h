#ifndef SIMPLECONTACTLIST_H
#define SIMPLECONTACTLIST_H

#include "libqutim/contactlist.h"
#include "libqutim/actiontoolbar.h"
#include "libqutim/status.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace SimpleContactList
	{
		struct ModulePrivate;

		class Module : public ContactList
		{
			Q_OBJECT
			Q_PROPERTY(QWidget* widget READ widget)
		public:
			Module();
			virtual ~Module();
			virtual void addContact(ChatUnit *contact);
			virtual void removeContact(ChatUnit *contact);
			virtual void removeAccount(Account *account);
			Q_INVOKABLE QWidget *widget();
		public slots:
			void show();
			void hide();
			void changeVisibility();
		private slots:
			void onConfigureClicked();
			void onAccountCreated(qutim_sdk_0_3::Account *account);
			void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
			void onStatusChanged();
			void onSearchButtonToggled(bool toggled);
		private:
			QAction *createGlobalStatusAction(Status::Type type); //FIXME, temporary. Will be replaced by metacontact manager
			QScopedPointer<ModulePrivate> p;
		};
	}
}

#endif // SIMPLECONTACTLIST_H
