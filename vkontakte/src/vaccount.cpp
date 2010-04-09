#include "vaccount.h"
#include <qutim/chatunit.h>
#include <qutim/passworddialog.h>
#include "vcontact.h"
#include "vkontakteprotocol.h"

struct VAccountPrivate
{
	QString name;
	QHash<QString, VContact*> contacts;
};

VAccount::VAccount(const QString& uid) : 
	Account(uid, VkontakteProtocol::instance()),
	d_ptr(new VAccountPrivate)
{

}

VContact* VAccount::getContact(const QString& uid, bool create)
{
	Q_D(VAccount);
	VContact *contact = d->contacts.value(uid);
	if (create && !contact) {
		contact = new VContact(uid, this);
		d->contacts.insert(uid, contact);
		emit contactCreated(contact);
	}
	return contact;
}

void VAccount::updateSettings()
{

}

VAccount::~VAccount()
{

}

QString VAccount::password()
{
	QString password = config().group("general").value("passwd", QString(), Config::Crypted);
	if (password.isEmpty()) {
		PasswordDialog *dialog = PasswordDialog::request(this);
		if (dialog->exec() == PasswordDialog::Accepted) {
			password = dialog->password();
			if (dialog->remember()) {
				config().group("general").setValue("passwd", password, Config::Crypted);
				config().sync();
			}
		}
		delete dialog;
	}
	return password;
}
