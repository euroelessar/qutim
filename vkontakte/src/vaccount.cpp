#include "vaccount.h"
#include <qutim/chatunit.h>
#include <qutim/passworddialog.h>
#include "vcontact.h"
#include "vkontakteprotocol.h"
#include "vconnection.h"
#include "vconnection_p.h"
#include "vroster.h"
#include "vaccount_p.h"
#include <qutim/contactlist.h>

VAccount::VAccount(const QString& email) : 
	Account(email, VkontakteProtocol::instance()),
	d_ptr(new VAccountPrivate)
{
	Q_D(VAccount);
	setParent(protocol());
	d->connection = new VConnection(this,this);
}

VContact* VAccount::getContact(const QString& uid, bool create)
{
	Q_D(VAccount);
	VContact *contact = d->contactsHash.value(uid);
	if (create && !contact) {
		contact = new VContact(uid, this);
		d->contactsHash.insert(uid, contact);
		d->contactsList.append(contact);
		emit contactCreated(contact);
	}
	return contact;
}

void VAccount::loadSettings()
{
	Q_D(VAccount);
	ConfigGroup contactList = config().group("contactList");
	for (int index=0;index!=contactList.arraySize();index++) {
		ConfigGroup item = contactList.at(index);
		QString id = item.value<QString>("id",QString());
		if (id.isEmpty())
			continue;
		VContact *c = getContact(id,true);
		c->setInList(item.value<bool>("inList",true));
		c->setName(item.value<QString>("name",id));
		c->setAvatar(item.group("avatar").value<QString>("path",QString()));
		c->setProperty("avatarUrl",item.group("avatar").value<QString>("url",QString()));
		c->setActivity(item.value<QString>("activity",QString()));
		debug() << "added contact:" << c->name() << "in list" << c->isInList();
		if (c->isInList())
			ContactList::instance()->addContact(c);
	}
}

void VAccount::saveSettings()
{
	Q_D(VAccount);
	ConfigGroup contactList = config().group("contactList");
	for (int i=0;i!=d->contactsList.count();i++) {
		ConfigGroup item = contactList.at(i);
		const VContact *c = d->contactsList.at(i);
		item.setValue("id",c->id());
		item.setValue("name",c->name());
		item.setValue("activity",c->status().text());
		item.setValue("inList",c->isInList());
		ConfigGroup avatar = item.group("avatar");
		avatar.setValue("url",c->property("avatarUrl"));
		avatar.setValue("path",c->avatar());
	}
	contactList.sync();
}

VAccount::~VAccount()
{
	saveSettings();
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
				saveSettings();
			break;
		}
		default: {
			break;
		}		
	}

	emit statusChanged(status);
}

VConnection *VAccount::connection()
{
	return d_func()->connection;
}

const VConnection *VAccount::connection() const
{
	return d_func()->connection;
}

