#include "passworddialog.h"
#include "account.h"

namespace qutim_sdk_0_3
{
	Q_GLOBAL_STATIC_WITH_ARGS(const ObjectGenerator*, data, (NULL))

	PasswordDialog *PasswordDialog::request(Account *account)
	{
		const ObjectGenerator * &gen = *data();
		if (!gen) {
			const GeneratorList list = moduleGenerators<PasswordDialog>();
			Q_ASSERT(!list.isEmpty());
			if (list.isEmpty())
				return NULL;
			gen = *list.begin();
		}
		PasswordDialog *dialog = gen->generate<PasswordDialog>();
		dialog->setAccount(account);
		return dialog;
	}

	PasswordDialog::PasswordDialog()
	{
	}

	void PasswordDialog::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}
