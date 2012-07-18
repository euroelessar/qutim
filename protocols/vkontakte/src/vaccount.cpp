/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#include <qutim/systemintegration.h>

#include "vcontact.h"
#include "vprotocol.h"
#include "vroster.h"
#include "vinforequest.h"

#include <vk/roster.h>
#include <vk/contact.h>

#include "vprotocol.h"
#include <vk/auth/oauthconnection.h>

#include <QWebView>

const static int qutimId = 1865463;

using namespace qutim_sdk_0_3;

VAccount::VAccount(const QString &email, VProtocol *protocol) :
	Account(email, protocol),
	m_client(new VClient(email, this))
{
	setObjectName("VAccount");

	connect(m_client, SIGNAL(connectionStateChanged(vk::Client::State)), SLOT(onClientStateChanged(vk::Client::State)));
	connect(m_client, SIGNAL(meChanged(vk::Buddy*)), SLOT(onMeChanged(vk::Buddy*)));
	connect(m_client, SIGNAL(invisibleChanged(bool)), SLOT(onInvisibleChanged(bool)));
	connect(m_client, SIGNAL(error(vk::Client::Error)), SLOT(onError(vk::Client::Error)));

	setInfoRequestFactory(new VInfoFactory(this));
	m_roster = new VRoster(this);
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
	if (Account::status().type() != status.type()) {
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
}

int VAccount::uid() const
{
	if (m_client->me())
		return m_client->me()->id();
	return 0;
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
	return m_roster;
}

vk::Client *VAccount::client() const
{
	return m_client;
}

VContact *VAccount::me() const
{
	return m_me.data();
}

void VAccount::loadSettings()
{
	Config cfg = config();
	m_name = cfg.value("general/name", QString());
	vk::OAuthConnection *connection = new vk::OAuthConnection(qutimId, this);
	connection->setUid(cfg.value("access/uid", 0));
	connection->setAccessToken(cfg.value("access/token", QByteArray(), Config::Crypted),
							   cfg.value("access/expires", 0));
	connect(connection, SIGNAL(authConfirmRequested(QWebPage*)), SLOT(onAuthConfirmRequested(QWebPage*)));
	connect(connection, SIGNAL(accessTokenChanged(QByteArray,time_t)), SLOT(setAccessToken(QByteArray,time_t)));
	m_client->setConnection(connection);
}

void VAccount::saveSettings()
{
	config().setValue("access/uid", uid());
	config().setValue("general/name", m_name);
	if (vk::OAuthConnection *c = qobject_cast<vk::OAuthConnection*>(m_client->connection()))
		setAccessToken(c->accessToken(), c->expiresIn());
}

VRoster *VAccount::roster() const
{
	return m_roster.data();
}

void VAccount::onMeChanged(vk::Buddy *me)
{
	if (m_me)
		m_me->deleteLater();
	m_me = new VContact(me, this);
	connect(m_me.data(), SIGNAL(nameChanged(QString,QString)), SIGNAL(nameChanged(QString,QString)));
}

void VAccount::onInvisibleChanged(bool set)
{
	if (m_client->connectionState() == vk::Client::StateOnline) {
		Status s = status();
		s.setType(set ? Status::Invisible : Status::Online);
		Account::setStatus(s);
	}
}

void VAccount::onAuthConfirmRequested(QWebPage *page)
{
	QWebView *view = new QWebView;
	view->setPage(page);
	connect(page, SIGNAL(destroyed()), view, SLOT(deleteLater()));
	SystemIntegration::show(view);
}

void VAccount::setAccessToken(const QByteArray &token, time_t expiresIn)
{
	Config cfg = config().group("access");
	cfg.setValue("token", token, Config::Crypted);
	cfg.setValue("expires", expiresIn);
}

void VAccount::onError(vk::Client::Error error)
{
	if (error == vk::Client::ErrorAuthorizationFailed)
		config("general").setValue("passwd", "");
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

