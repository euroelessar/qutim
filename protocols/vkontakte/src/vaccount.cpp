/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "vaccount.h"
#include <qutim/chatunit.h>
#include <qutim/passworddialog.h>
#include "vcontact.h"
#include "vkontakteprotocol.h"
#include "vconnection.h"
#include "vconnection_p.h"
#include "vroster.h"
#include "vaccount_p.h"
#include <qutim/inforequest.h>
#include "vinforequest.h"

#include <roster.h>
#include <contact.h>

#include "vprotocol.h"

VAccount::VAccount(const QString& email,QObject *parent) :
	Account(email, VkontakteProtocol::instance()),
	d_ptr(new VAccountPrivate)
{
	setInfoRequestFactory(new VInfoFactory(this));
	Q_D(VAccount);
	d->q_ptr = this;
	setParent(protocol());
	d->connection = new VConnection(this,this);
	setStatus(Status::instance(Status::Offline,"vkontakte"));
}

VContact* VAccount::getContact(const QString& uid, bool create)
{
	return d_func()->connection->roster()->getContact(uid, create);
}

ChatUnit* VAccount::getUnit(const QString& unitId, bool create)
{
	return getContact(unitId,create);
}

void VAccount::loadSettings()
{
	d_func()->name = config().value("general/name", QString());
}

void VAccount::saveSettings()
{
	d_func()->connection->saveSettings();
}

VAccount::~VAccount()
{
	//	saveSettings();
}

QString VAccount::name() const
{
	return d_func()->name;
}

void VAccount::setAccountName(const QString &name)
{
	Q_D(VAccount);
	if (d->name != name) {
		QString previous = d->name;
		d->name = name;
		config().setValue("general/name", name);
		emit nameChanged(name, previous);
	}
}

QString VAccount::password()
{
	Config cfg = config("general");
	QString password = cfg.value("passwd", QString(), Config::Crypted);
	if (password.isEmpty()) {
		PasswordDialog *dialog = PasswordDialog::request(this);
		if (dialog->exec() == PasswordDialog::Accepted) {
			password = dialog->password();
			if (dialog->remember())
				cfg.setValue("passwd", password, Config::Crypted);
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
	Q_D(VAccount);
	VConnectionState state = statusToState(status.type());

	switch (state) {
	case Connected: {
		if (d->connection->connectionState() == Disconnected)
			d->connection->connectToHost();
		else if(d->connection->connectionState() == Connected)
			d->connection->roster()->setActivity(status);
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
	Account::setStatus(status);
}

VConnection *VAccount::connection()
{
	return d_func()->connection;
}

const VConnection *VAccount::connection() const
{
	return d_func()->connection;
}

VContactList VAccount::contacts() const
{
	return findChildren<VContact*>();
}

namespace playground {

VAccount::VAccount(const QString &email, VProtocol *protocol) :
	Account(email, protocol),
	m_client(new VClient(email, this))
{
	connect(m_client, SIGNAL(connectionStateChanged(vk::Client::State)), SLOT(onClientStateChanged(vk::Client::State)));
}

QString VAccount::name() const
{
	if (!m_name.isEmpty())
		return m_name;
	return Account::name();
}

void VAccount::setStatus(Status status)
{
	switch (status.type()) {
	case Status::Offline:
		m_client->disconnectFromHost();
		saveSettings();
		if (status.changeReason() == Status::ByAuthorizationFailed)
			config("general").setValue("passwd", "");
		break;
	case Status::Connecting:
		break;
	default:
		m_client->setPassword(requestPassword());
		m_client->connectToHost();
	};
}

int VAccount::uid() const
{
	return m_client->me()->id();
}

QString VAccount::email() const
{
	return m_client->login();
}

void VAccount::loadSettings()
{
	m_name = config().value("general/name", QString());
}

void VAccount::saveSettings()
{
	//TODO
}

QString VAccount::requestPassword()
{
	Config cfg = config("general");
	QString password = cfg.value("passwd", QString(), Config::Crypted);
	if (password.isEmpty()) {
		PasswordDialog *dialog = PasswordDialog::request(this);
		if (dialog->exec() == PasswordDialog::Accepted) {
			password = dialog->password();
			if (dialog->remember())
				cfg.setValue("passwd", password, Config::Crypted);
		}
		dialog->deleteLater();
	}
	return password;
}

void VAccount::onClientStateChanged(vk::Client::State state)
{
	Status status;
	switch (state) {
	case vk::Client::StateOffline:
		status.setType(Status::Offline);
		break;
	case vk::Client::StateInvisible:
		status.setType(Status::Invisible);
		break;
	case vk::Client::StateConnecting:
		status.setType(Status::Connecting);
		break;
	case vk::Client::StateOnline:
		status.setType(Status::Online);
		break;
	default:
		break;
	}
	Account::setStatus(status);
}

void VAccount::setAccountName(const QString &name)
{
	m_name = name;
	QString old = m_name;
	emit nameChanged(name, old);
}

ChatUnit *VAccount::getUnit(const QString &unitId, bool create)
{
	return 0;
}

} //namespace playground
