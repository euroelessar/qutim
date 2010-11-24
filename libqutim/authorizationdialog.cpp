#include "authorizationdialog.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
Q_GLOBAL_STATIC_WITH_ARGS(const ObjectGenerator*, data, (NULL))

class AuthorizationDialogPrivate
{
public:
	Contact *contact;
};

AuthorizationDialog::~AuthorizationDialog()
{
}

AuthorizationDialog *AuthorizationDialog::request(Contact *contact, const QString &text, bool incoming)
{
	const ObjectGenerator * &gen = *data();
	if (!gen) {
		const GeneratorList list = ObjectGenerator::module<AuthorizationDialog>();
		Q_ASSERT(!list.isEmpty());
		if (list.isEmpty())
			return NULL;
		gen = *list.begin();
	}
	AuthorizationDialog *dialog = gen->generate<AuthorizationDialog>();
	dialog->d->contact = contact;
	dialog->setContact(contact, text, incoming);
	return dialog;
}

Contact *AuthorizationDialog::contact() const
{
	return d->contact;
}

AuthorizationDialog::AuthorizationDialog() :
	d(new AuthorizationDialogPrivate)
{
}

void AuthorizationDialog::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}

QEvent::Type AuthEvent::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 103));
	return type;
}

}
