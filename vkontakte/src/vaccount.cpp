#include "vaccount.h"
#include <qutim/chatunit.h>
#include <qutim/passworddialog.h>
#include "vcontact.h"
#include "vkontakteprotocol.h"
#include "vconnection.h"
#include "vconnection_p.h"
#include "vroster.h"

struct VAccountPrivate
{
	QString name;
	QString uid;
	QHash<QString, VContact*> contacts;
	VConnection *connection;
};

VAccount::VAccount(const QString& email) : 
	Account(email, VkontakteProtocol::instance()),
	d_ptr(new VAccountPrivate)
{
	Q_D(VAccount);
	d->connection = new VConnection(this,this);
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
		dialog->deleteLater();
	}
	return password;
}

QString VAccount::uid() const
{
	return d_func()->uid;
}

void VAccount::setUid(const QString& uid)
{
	d_func()->uid = uid;
}

void VAccount::setStatus(Status status)
{
	Account::setStatus(status);
	Q_D(VAccount);
	status.initIcon("vkontakte");
	VConnectionState state = statusToState(status.type());

	switch (state) {
		case Connected: {
			if (d->connection->connectionState() == Disconnected)
				d->connection->connectToHost(password());
			break;
		}
		case Disconnected: {
			if (d->connection->connectionState() != Disconnected)
				d->connection->disconnectFromHost();
			break;
		}
		default: {
			break;
		}		
	}
}

VConnection *VAccount::connection()
{
	return d_func()->connection;
}

const VConnection *VAccount::connection() const
{
	return d_func()->connection;
}

