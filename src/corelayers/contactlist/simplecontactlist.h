#ifndef SIMPLECONTACTLIST_H
#define SIMPLECONTACTLIST_H

#include "libqutim/contactlist.h"

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace SimpleContactList
	{
		struct ModulePrivate;

		class Module : public ContactList
		{
			Q_OBJECT
		public:
			Module();
			virtual ~Module();
			virtual void addContact(ChatUnit *contact);
			virtual void removeContact(ChatUnit *contact);
			virtual void removeAccount(Account *account);
		private slots:
			void onConfigureClicked();
		private:
			QScopedPointer<ModulePrivate> p;
		};
	}
}

#endif // SIMPLECONTACTLIST_H
