#include "authorizationdialog.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	Q_GLOBAL_STATIC_WITH_ARGS(const ObjectGenerator*, data, (NULL))

	AuthorizationDialog *AuthorizationDialog::request(Contact *contact, const QString &text)
	{
		const ObjectGenerator * &gen = *data();
		if (!gen) {
			const GeneratorList list = moduleGenerators<AuthorizationDialog>();
			Q_ASSERT(!list.isEmpty());
			if (list.isEmpty())
				return NULL;
			gen = *list.begin();
		}
		AuthorizationDialog *dialog = gen->generate<AuthorizationDialog>();
		dialog->setContact(contact, text);
		return dialog;
	}

	AuthorizationDialog::AuthorizationDialog()
	{
	}

	void AuthorizationDialog::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}
