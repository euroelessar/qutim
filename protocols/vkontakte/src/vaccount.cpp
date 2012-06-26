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
	connect(m_client, SIGNAL(meChanged(vk::Contact*)), SLOT(onMeChanged(vk::Contact*)));
	connect(m_client, SIGNAL(invisibleChanged(bool)), SLOT(onInvisibleChanged(bool)));

	setInfoRequestFactory(new VInfoFactory(this));
	VAccount::setStatus(Status::instance(Status::Offline, "vkontakte"));
}

QString VAccount::name() const
{
	if (m_client->me())
		return m_client->me()->name();
	return email();
}

void VAccount::setStatus(Status status)
{
	m_client->setActivity(status.text());
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
		m_client->setInvisible(status == Status::Invisible);
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

VRoster *VAccount::roster()
{
	if (m_roster.isNull())
		m_roster = new VRoster(this);
	return m_roster;
}

vk::Client *VAccount::client() const
{
	return m_client;
}

void VAccount::loadSettings()
{
	m_name = config().value("general/name", QString());
}

void VAccount::saveSettings()
{
	//TODO
}

VRoster *VAccount::roster() const
{
	return m_roster.data();
}

void VAccount::onNameChanged(const QString &name)
{
	m_name = name;
	QString old = m_name;
	emit nameChanged(name, old);
}

void VAccount::onMeChanged(vk::Contact *me)
{
	connect(me, SIGNAL(nameChanged(QString)), SLOT(onNameChanged(QString)));
}

void VAccount::onInvisibleChanged(bool set)
{
	if (m_client->connectionState() == vk::Client::StateOnline) {
		Status s = status();
		s.setType(set ? Status::Invisible : Status::Online);
		Account::setStatus(s);
	}
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
	Status s = status();
	switch (state) {
	case vk::Client::StateOffline:
		s.setType(Status::Offline);
		break;
	case vk::Client::StateConnecting:
		s.setType(Status::Connecting);
		break;
	case vk::Client::StateOnline:
		s.setType(m_client->isInvisible() ? Status::Invisible : Status::Online);
		break;
	default:
		break;
	}
	Account::setStatus(s);

	if (m_client->isOnline())
		m_client->roster()->sync();
}

ChatUnit *VAccount::getUnit(const QString &unitId, bool create)
{
	return roster()->contact(unitId.toInt(), create);
}

