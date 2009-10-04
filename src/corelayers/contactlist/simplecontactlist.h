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
			virtual void addContact(Contact *contact);
			virtual void removeContact(Contact *contact);
			virtual void removeAccount(Account *account);
		private:
			QScopedPointer<ModulePrivate> p;
		};
	}
}

#endif // SIMPLECONTACTLIST_H
