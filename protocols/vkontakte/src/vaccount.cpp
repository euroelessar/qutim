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
#include "vprotocol.h"
#include "vroster.h"
#include "vinforequest.h"

#include <vk/roster.h>
#include <vk/contact.h>

#include "vprotocol.h"

using namespace qutim_sdk_0_3;

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

vk::Connection *VAccount::connection() const
{
	return m_client->connection();
}

vk::Roster *VAccount::roster() const
{
	return m_client->roster();
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

